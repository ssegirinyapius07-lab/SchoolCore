#pragma once

#include "DbConnection.h"
#include "ThemeManager.h"
#include "AuthSession.h"

#include <mariadb/conncpp.hpp>
#include <msclr/marshal_cppstd.h>

#include <memory>
#include <string>

namespace SchoolCore
{
    using namespace System;
    using namespace System::Drawing;
    using namespace System::Data;
    using namespace System::Security::Cryptography;
    using namespace System::Windows::Forms;

    public ref class UsersRoles : public Form
    {
    public:
        UsersRoles(void)
        {
            InitializeComponent();
                                ThemeManager::ApplyToForm(this);
                    EnsureUserEmailColumn();
            LoadRoles();
            LoadUsers();
            ApplyPermissions();
        }

        System::ComponentModel::Container^ components;

    protected:
        ~UsersRoles(void)
        {
            if (components)
            {
                delete components;
            }
        }

    private:
        Panel^ headerPanel;
        Label^ lblTitle;
        Label^ lblSubtitle;

        Panel^ toolbarPanel;
        Button^ btnAddUser;
        Button^ btnEditUser;
        Button^ btnToggleUser;
        Button^ btnResetPassword;
        Label^ lblSearch;
        TextBox^ txtSearch;
        Button^ btnSearch;

        DataGridView^ usersGrid;
        Label^ lblUserCount;

        Panel^ rolePanel;
        Label^ lblRoleTitle;
        Label^ lblRoleName;
        Label^ lblRoleDescription;
        Label^ lblPermissionsTitle;
        ListBox^ lstPermissions;

        Button^ btnClose;

        ComboBox^ cmbRole;
        TextBox^ txtUsername;
        TextBox^ txtFullName;
        Form^ editorForm;
        ComboBox^ editorRoleBox;

        bool editMode = false;
        int editingUserId = 0;
        bool emailColumnAvailable = false;
        DataTable^ roleTable = nullptr;

        static String^ Base64Encode(array<Byte>^ value)
        {
            return Convert::ToBase64String(value);
        }

        static String^ HashPassword(
            String^ password,
            array<Byte>^ salt,
            int iterations)
        {
            Rfc2898DeriveBytes^ derive =
                gcnew Rfc2898DeriveBytes(
                    password,
                    salt,
                    iterations,
                    HashAlgorithmName::SHA256
                );

            array<Byte>^ hash = derive->GetBytes(32);
            delete derive;

            return Base64Encode(hash);
        }

        static String^ CreatePasswordHash(String^ password)
        {
            const int iterations = 100000;

            array<Byte>^ salt = gcnew array<Byte>(16);
            RandomNumberGenerator^ rng =
                RandomNumberGenerator::Create();

            rng->GetBytes(salt);
            delete rng;

            return L"PBKDF2-SHA256$" +
                iterations.ToString() +
                L"$" +
                Base64Encode(salt) +
                L"$" +
                HashPassword(password, salt, iterations);
        }

        static String^ GenerateTemporaryPassword()
        {
            String^ chars =
                L"ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz23456789";

            array<Byte>^ bytes = gcnew array<Byte>(12);
            RandomNumberGenerator^ rng =
                RandomNumberGenerator::Create();

            rng->GetBytes(bytes);
            delete rng;

            String^ password = L"";

            for (int i = 0; i < bytes->Length; i++)
            {
                password += chars[
                    bytes[i] % chars->Length
                ];
            }

            return password;
        }

        String^ GetSelectedUsername()
        {
            if (usersGrid->CurrentRow == nullptr)
            {
                return L"";
            }

            Object^ value =
                usersGrid->CurrentRow->Cells[L"Username"]->Value;

            return value == nullptr
                ? L""
                : value->ToString();
        }

        int GetSelectedUserId()
        {
            if (usersGrid->CurrentRow == nullptr)
            {
                return 0;
            }

            Object^ value =
                usersGrid->CurrentRow->Cells[L"UserId"]->Value;

            if (value == nullptr)
            {
                return 0;
            }

            int id = 0;
            Int32::TryParse(value->ToString(), id);
            return id;
        }

        int GetSelectedRoleId()
        {
            if (cmbRole == nullptr || cmbRole->SelectedItem == nullptr)
            {
                return 0;
            }

            DataRowView^ row =
                dynamic_cast<DataRowView^>(
                    cmbRole->SelectedItem
                );

            if (row == nullptr)
            {
                return 0;
            }

            return Convert::ToInt32(
                row[L"role_id"]
            );
        }

        void ApplyPermissions()
        {
            bool canManage =
                AuthSession::HasPermission(
                    L"users.manage"
                );

            btnAddUser->Enabled = canManage;
            btnEditUser->Enabled = canManage;
            btnToggleUser->Enabled = canManage;
            btnResetPassword->Enabled = canManage;
        }

        void EnsureUserEmailColumn()
        {
            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> check(
                    con->prepareStatement(
                        "SELECT COUNT(*) "
                        "FROM information_schema.COLUMNS "
                        "WHERE TABLE_SCHEMA = DATABASE() "
                        "AND TABLE_NAME = 'users' "
                        "AND COLUMN_NAME = 'email'"
                    )
                );

                std::unique_ptr<sql::ResultSet> result(
                    check->executeQuery()
                );

                bool exists = false;

                if (result->next())
                {
                    exists = result->getInt(1) > 0;
                }

                if (!exists)
                {
                    std::unique_ptr<sql::Statement> alter(
                        con->createStatement()
                    );

                    alter->execute(
                        "ALTER TABLE users "
                        "ADD COLUMN email VARCHAR(190) NULL "
                        "AFTER username"
                    );
                }

                emailColumnAvailable = true;
            }
            catch (sql::SQLException& ex)
            {
                emailColumnAvailable = false;

                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"User Database Update",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
            }
            catch (Exception^)
            {
                emailColumnAvailable = false;
            }
        }

        void LoadRoles()
        {
            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::Statement> stmt(
                    con->createStatement()
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery(
                        "SELECT role_id, role_name, "
                        "COALESCE(description, '') AS description "
                        "FROM roles ORDER BY role_name"
                    )
                );

                DataTable^ table = gcnew DataTable();
                table->Columns->Add(L"role_id", Int32::typeid);
                table->Columns->Add(L"role_name", String::typeid);
                table->Columns->Add(L"description", String::typeid);

                while (result->next())
                {
                    DataRow^ row = table->NewRow();

                    row[L"role_id"] =
                        result->getInt("role_id");

                    row[L"role_name"] =
                        gcnew String(
                            result->getString(
                                "role_name"
                            ).c_str()
                        );

                    row[L"description"] =
                        gcnew String(
                            result->getString(
                                "description"
                            ).c_str()
                        );

                    table->Rows->Add(row);
                }

                roleTable = table;
                cmbRole->DataSource = roleTable;
                cmbRole->DisplayMember = L"role_name";
                cmbRole->ValueMember = L"role_id";

                if (cmbRole->Items->Count > 0)
                {
                    cmbRole->SelectedIndex = 0;
                }
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
            catch (Exception^ ex)
            {
                MessageBox::Show(
                    ex->Message,
                    L"Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }

        void LoadUsers()
        {
            try
            {
                auto con = DbConnection::GetConnection();

                String^ search =
                    txtSearch->Text->Trim();

                String^ pattern =
                    L"%" + search + L"%";

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT u.user_id, u.username, "
                        "u.full_name, "
                        "COALESCE(u.email, '') AS email, "
                        "r.role_name, "
                        "u.status, u.must_change_password "
                        "FROM users u "
                        "INNER JOIN roles r ON r.role_id = u.role_id "
                        "WHERE (? = '' OR u.username LIKE ? "
                        "OR u.full_name LIKE ? "
                        "OR COALESCE(u.email, '') LIKE ? "
                        "OR r.role_name LIKE ?) "
                        "ORDER BY u.user_id DESC"
                    )
                );

                std::string patternText =
                    msclr::interop::marshal_as<std::string>(
                        pattern
                    );

                std::string searchText =
                    msclr::interop::marshal_as<std::string>(
                        search
                    );

                stmt->setString(1, searchText);
                stmt->setString(2, patternText);
                stmt->setString(3, patternText);
                stmt->setString(4, patternText);
                stmt->setString(5, patternText);

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                usersGrid->Rows->Clear();

                while (result->next())
                {
                    int rowIndex =
                        usersGrid->Rows->Add();

                    usersGrid->Rows[rowIndex]
                        ->Cells[L"UserId"]->Value =
                        result->getInt("user_id");

                    usersGrid->Rows[rowIndex]
                        ->Cells[L"Username"]->Value =
                        gcnew String(
                            result->getString(
                                "username"
                            ).c_str()
                        );

                    usersGrid->Rows[rowIndex]
                        ->Cells[L"FullName"]->Value =
                        gcnew String(
                            result->getString(
                                "full_name"
                            ).c_str()
                        );

                    usersGrid->Rows[rowIndex]
                        ->Cells[L"Email"]->Value =
                        gcnew String(
                            result->getString(
                                "email"
                            ).c_str()
                        );

                    usersGrid->Rows[rowIndex]
                        ->Cells[L"Role"]->Value =
                        gcnew String(
                            result->getString(
                                "role_name"
                            ).c_str()
                        );

                    usersGrid->Rows[rowIndex]
                        ->Cells[L"Status"]->Value =
                        gcnew String(
                            result->getString(
                                "status"
                            ).c_str()
                        );

                    bool mustChange =
                        result->getBoolean(
                            "must_change_password"
                        );

                    usersGrid->Rows[rowIndex]
                        ->Cells[L"PasswordStatus"]->Value =
                        mustChange
                        ? L"Temporary"
                        : L"Set";
                }

                lblUserCount->Text =
                    L"Users: " +
                    usersGrid->Rows->Count.ToString();
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
            catch (Exception^ ex)
            {
                MessageBox::Show(
                    ex->Message,
                    L"Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }

        void LoadRoleDetails()
        {
            if (usersGrid->CurrentRow == nullptr)
            {
                lblRoleName->Text = L"No role selected";
                lblRoleDescription->Text = L"";
                lstPermissions->Items->Clear();
                return;
            }

            String^ roleName =
                usersGrid->CurrentRow
                    ->Cells[L"Role"]->Value == nullptr
                ? L""
                : usersGrid->CurrentRow
                    ->Cells[L"Role"]->Value->ToString();

            lblRoleName->Text =
                String::IsNullOrWhiteSpace(roleName)
                ? L"No role selected"
                : roleName;

            lstPermissions->Items->Clear();

            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT r.description "
                        "FROM roles r "
                        "WHERE r.role_name = ? "
                        "LIMIT 1"
                    )
                );

                stmt->setString(
                    1,
                    msclr::interop::marshal_as<std::string>(
                        roleName
                    )
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                if (result->next())
                {
                    lblRoleDescription->Text =
                        gcnew String(
                            result->getString(
                                "description"
                            ).c_str()
                        );
                }
                else
                {
                    lblRoleDescription->Text = L"";
                }

                stmt.reset(
                    con->prepareStatement(
                        "SELECT p.permission_name "
                        "FROM roles r "
                        "INNER JOIN role_permissions rp "
                        "ON rp.role_id = r.role_id "
                        "INNER JOIN permissions p "
                        "ON p.permission_id = rp.permission_id "
                        "WHERE r.role_name = ? "
                        "ORDER BY p.permission_id"
                    )
                );

                stmt->setString(
                    1,
                    msclr::interop::marshal_as<std::string>(
                        roleName
                    )
                );

                result.reset(
                    stmt->executeQuery()
                );

                while (result->next())
                {
                    lstPermissions->Items->Add(
                        gcnew String(
                            result->getString(
                                "permission_name"
                            ).c_str()
                        )
                    );
                }
            }
            catch (Exception^ ex)
            {
                lblRoleDescription->Text =
                    ex->Message;
            }
        }

        System::Void SaveUserEditor(
            System::Object^ sender,
            System::EventArgs^ e
        )
        {
            String^ username =
                txtUsername->Text->Trim();

            String^ fullName =
                txtFullName->Text->Trim();

            if (String::IsNullOrWhiteSpace(username) ||
                String::IsNullOrWhiteSpace(fullName) ||
                editorRoleBox == nullptr ||
                editorRoleBox->SelectedValue == nullptr)
            {
                MessageBox::Show(
                    L"Username, full name and role are required.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                return;
            }

            try
            {
                auto con =
                    DbConnection::GetConnection();

                int roleId =
                    Convert::ToInt32(
                        editorRoleBox->SelectedValue
                    );

                if (!editMode)
                {
                    String^ temporaryPassword =
                        GenerateTemporaryPassword();

                    String^ hash =
                        CreatePasswordHash(
                            temporaryPassword
                        );

                    std::unique_ptr<sql::PreparedStatement> stmt(
                        con->prepareStatement(
                            "INSERT INTO users "
                            "(username, password_hash, full_name, "
                            "role_id, status, must_change_password) "
                            "VALUES (?, ?, ?, ?, 'Active', 1)"
                        )
                    );

                    stmt->setString(
                        1,
                        msclr::interop::marshal_as<std::string>(
                            username
                        )
                    );

                    stmt->setString(
                        2,
                        msclr::interop::marshal_as<std::string>(
                            hash
                        )
                    );

                    stmt->setString(
                        3,
                        msclr::interop::marshal_as<std::string>(
                            fullName
                        )
                    );

                    stmt->setInt(4, roleId);
                    stmt->execute();

                    MessageBox::Show(
                        L"User created successfully.\\n\\n"
                        L"Username: " + username +
                        L"\\nTemporary password: " +
                        temporaryPassword +
                        L"\\n\\n"
                        L"Give this temporary password to the user. "
                        L"It will not be shown again.",
                        L"User Created",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Information
                    );
                }
                else
                {
                    std::unique_ptr<sql::PreparedStatement> stmt(
                        con->prepareStatement(
                            "UPDATE users "
                            "SET full_name = ?, role_id = ? "
                            "WHERE user_id = ?"
                        )
                    );

                    stmt->setString(
                        1,
                        msclr::interop::marshal_as<std::string>(
                            fullName
                        )
                    );

                    stmt->setInt(2, roleId);
                    stmt->setInt(3, editingUserId);
                    stmt->execute();

                    MessageBox::Show(
                        L"User details updated successfully.",
                        L"User Updated",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Information
                    );
                }

                editorForm->DialogResult =
                    System::Windows::Forms::DialogResult::OK;

                editorForm->Close();
            }
            catch (sql::SQLException& ex)
            {
                String^ message =
                    gcnew String(ex.what());

                if (message->IndexOf(
                        L"Duplicate",
                        StringComparison::OrdinalIgnoreCase
                    ) >= 0)
                {
                    message =
                        L"The username is already in use.";
                }

                MessageBox::Show(
                    message,
                    L"Unable to Save User",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
            catch (Exception^ ex)
            {
                MessageBox::Show(
                    ex->Message,
                    L"Unable to Save User",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }

        void ShowUserEditor(
            bool edit,
            int userId)
        {
            editMode = edit;
            editingUserId = userId;

            editorForm =
                gcnew Form();

            editorForm->Text =
                edit
                ? L"Edit User"
                : L"Add User";

            editorForm->StartPosition =
                FormStartPosition::CenterParent;

            editorForm->FormBorderStyle =
                System::Windows::Forms::FormBorderStyle::FixedDialog;

            editorForm->MaximizeBox = false;
            editorForm->MinimizeBox = false;
            editorForm->ClientSize =
                Drawing::Size(500, 330);

            TableLayoutPanel^ layout =
                gcnew TableLayoutPanel();

            layout->Dock = DockStyle::Fill;
            layout->Padding =
                System::Windows::Forms::Padding(28, 24, 28, 24);
            layout->ColumnCount = 2;
            layout->RowCount = 5;

            layout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Absolute,
                    125
                )
            );

            layout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    100
                )
            );

            Label^ lblUser =
                gcnew Label();
            lblUser->Text = L"Username";
            lblUser->Dock = DockStyle::Fill;
            lblUser->TextAlign =
                ContentAlignment::MiddleLeft;

            txtUsername =
                gcnew TextBox();
            txtUsername->Dock = DockStyle::Fill;
            txtUsername->MaxLength = 50;

            Label^ lblName =
                gcnew Label();
            lblName->Text = L"Full name";
            lblName->Dock = DockStyle::Fill;
            lblName->TextAlign =
                ContentAlignment::MiddleLeft;

            txtFullName =
                gcnew TextBox();
            txtFullName->Dock = DockStyle::Fill;
            txtFullName->MaxLength = 150;

            Label^ lblRole =
                gcnew Label();
            lblRole->Text = L"Role";
            lblRole->Dock = DockStyle::Fill;
            lblRole->TextAlign =
                ContentAlignment::MiddleLeft;

            editorRoleBox =
                gcnew ComboBox();
            editorRoleBox->Dock = DockStyle::Fill;
            editorRoleBox->DropDownStyle =
                ComboBoxStyle::DropDownList;
            editorRoleBox->DataSource =
                roleTable;
            editorRoleBox->DisplayMember =
                L"role_name";
            editorRoleBox->ValueMember =
                L"role_id";

            Label^ lblPasswordInfo =
                gcnew Label();
            lblPasswordInfo->Text =
                edit
                ? L"Password"
                : L"Temporary password";
            lblPasswordInfo->Dock = DockStyle::Fill;
            lblPasswordInfo->TextAlign =
                ContentAlignment::MiddleLeft;

            Label^ passwordInfo =
                gcnew Label();
            passwordInfo->Text =
                edit
                ? L"Unchanged. Use Reset Password to issue a temporary password."
                : L"A secure temporary password will be generated.";
            passwordInfo->Dock = DockStyle::Fill;
            passwordInfo->AutoEllipsis = true;
            passwordInfo->ForeColor =
                Color::FromArgb(71, 85, 105);

            FlowLayoutPanel^ buttons =
                gcnew FlowLayoutPanel();
            buttons->Dock = DockStyle::Fill;
            buttons->FlowDirection =
                FlowDirection::RightToLeft;

            Button^ save =
                gcnew Button();
            save->Text =
                edit
                ? L"Save Changes"
                : L"Create User";
            save->Width = 125;
            save->Height = 36;

            Button^ cancel =
                gcnew Button();
            cancel->Text = L"Cancel";
            cancel->Width = 90;
            cancel->Height = 36;
            cancel->DialogResult =
                System::Windows::Forms::DialogResult::Cancel;

            save->Click +=
                gcnew EventHandler(
                    this,
                    &UsersRoles::SaveUserEditor
                );

            buttons->Controls->Add(save);
            buttons->Controls->Add(cancel);

            layout->Controls->Add(lblUser, 0, 0);
            layout->Controls->Add(txtUsername, 1, 0);
            layout->Controls->Add(lblName, 0, 1);
            layout->Controls->Add(txtFullName, 1, 1);
            layout->Controls->Add(lblRole, 0, 2);
            layout->Controls->Add(editorRoleBox, 1, 2);
            layout->Controls->Add(lblPasswordInfo, 0, 3);
            layout->Controls->Add(passwordInfo, 1, 3);
            layout->Controls->Add(buttons, 0, 4);
            layout->SetColumnSpan(buttons, 2);

            editorForm->Controls->Add(layout);
            editorForm->AcceptButton = save;
            editorForm->CancelButton = cancel;

            if (edit)
            {
                try
                {
                    auto con =
                        DbConnection::GetConnection();

                    std::unique_ptr<sql::PreparedStatement> stmt(
                        con->prepareStatement(
                            "SELECT username, full_name, role_id "
                            "FROM users WHERE user_id = ?"
                        )
                    );

                    stmt->setInt(1, userId);

                    std::unique_ptr<sql::ResultSet> result(
                        stmt->executeQuery()
                    );

                    if (result->next())
                    {
                        txtUsername->Text =
                            gcnew String(
                                result->getString(
                                    "username"
                                ).c_str()
                            );

                        txtFullName->Text =
                            gcnew String(
                                result->getString(
                                    "full_name"
                                ).c_str()
                            );

                        editorRoleBox->SelectedValue =
                            result->getInt("role_id");
                    }
                }
                catch (Exception^ ex)
                {
                    MessageBox::Show(
                        ex->Message,
                        L"Error",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Error
                    );

                    delete editorForm;
                    editorForm = nullptr;
                    return;
                }

                txtUsername->Enabled = false;
            }

            editorForm->ShowDialog(this);

            if (editorForm->DialogResult ==
                System::Windows::Forms::DialogResult::OK)
            {
                LoadUsers();
            }

            delete editorForm;
            editorForm = nullptr;
            editorRoleBox = nullptr;
        }

        void ToggleSelectedUser()
        {
            int userId = GetSelectedUserId();

            if (userId <= 0)
            {
                MessageBox::Show(
                    L"Select a user first.",
                    L"Users & Roles",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information
                );
                return;
            }

            if (userId == AuthSession::UserId)
            {
                MessageBox::Show(
                    L"You cannot deactivate the account currently in use.",
                    L"Users & Roles",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                return;
            }

            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "UPDATE users "
                        "SET status = CASE "
                        "WHEN status = 'Active' THEN 'Inactive' "
                        "ELSE 'Active' END "
                        "WHERE user_id = ?"
                    )
                );

                stmt->setInt(1, userId);
                stmt->execute();

                LoadUsers();
            }
            catch (Exception^ ex)
            {
                MessageBox::Show(
                    ex->Message,
                    L"Unable to Update Account",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }

        void ResetSelectedPassword()
        {
            int userId = GetSelectedUserId();

            if (userId <= 0)
            {
                MessageBox::Show(
                    L"Select a user first.",
                    L"Users & Roles",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information
                );
                return;
            }

            String^ username = GetSelectedUsername();

            if (MessageBox::Show(
                    L"Reset the password for " + username +
                    L"?\n\nA new temporary password will be generated "
                    L"and the user will be required to change it.",
                    L"Reset Password",
                    MessageBoxButtons::YesNo,
                    MessageBoxIcon::Question
                ) != System::Windows::Forms::DialogResult::Yes)
            {
                return;
            }

            try
            {
                String^ temporaryPassword =
                    GenerateTemporaryPassword();

                String^ hash =
                    CreatePasswordHash(
                        temporaryPassword
                    );

                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "UPDATE users "
                        "SET password_hash = ?, "
                        "must_change_password = 1, "
                        "password_changed_at = NULL "
                        "WHERE user_id = ?"
                    )
                );

                stmt->setString(
                    1,
                    msclr::interop::marshal_as<std::string>(
                        hash
                    )
                );

                stmt->setInt(2, userId);
                stmt->execute();

                MessageBox::Show(
                    L"Temporary password for " + username +
                    L":\n\n" +
                    temporaryPassword +
                    L"\n\n"
                    L"Give it to the user securely. "
                    L"It will not be shown again.",
                    L"Password Reset",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information
                );

                LoadUsers();
            }
            catch (Exception^ ex)
            {
                MessageBox::Show(
                    ex->Message,
                    L"Unable to Reset Password",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }

        System::Void btnAddUser_Click(
            Object^ sender,
            EventArgs^ e)
        {
            ShowUserEditor(false, 0);
        }

        System::Void btnEditUser_Click(
            Object^ sender,
            EventArgs^ e)
        {
            int userId = GetSelectedUserId();

            if (userId <= 0)
            {
                MessageBox::Show(
                    L"Select a user first.",
                    L"Users & Roles",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information
                );
                return;
            }

            ShowUserEditor(true, userId);
        }

        System::Void btnToggleUser_Click(
            Object^ sender,
            EventArgs^ e)
        {
            ToggleSelectedUser();
        }

        System::Void btnResetPassword_Click(
            Object^ sender,
            EventArgs^ e)
        {
            ResetSelectedPassword();
        }

        System::Void btnSearch_Click(
            Object^ sender,
            EventArgs^ e)
        {
            LoadUsers();
        }

        System::Void txtSearch_KeyDown(
            Object^ sender,
            KeyEventArgs^ e)
        {
            if (e->KeyCode == Keys::Enter)
            {
                LoadUsers();
                e->SuppressKeyPress = true;
            }
        }

        System::Void usersGrid_SelectionChanged(
            Object^ sender,
            EventArgs^ e)
        {
            LoadRoleDetails();
        }

        System::Void btnClose_Click(
            Object^ sender,
            EventArgs^ e)
        {
            Close();
        }

        void StyleButton(
            Button^ button,
            Color backColor)
        {
            button->BackColor = backColor;
            button->ForeColor = Color::White;
            button->FlatStyle = FlatStyle::Flat;
            button->FlatAppearance->BorderSize = 0;
            button->Font =
                gcnew Drawing::Font(
                    L"Segoe UI",
                    9.5F,
                    FontStyle::Bold
                );
            button->Height = 36;
            button->Margin =
                System::Windows::Forms::Padding(4, 0, 4, 0);
        }

        #pragma region Windows Form Designer generated code

        void InitializeComponent(void)
        {
            components =
                gcnew System::ComponentModel::Container();

            this->SuspendLayout();

            this->Text =
                L"SchoolCore | Users & Roles";

            this->StartPosition =
                FormStartPosition::CenterParent;

            this->BackColor =
                Color::FromArgb(241, 245, 249);

            this->Font =
                gcnew Drawing::Font(
                    L"Segoe UI",
                    9.5F
                );

            this->ClientSize =
                Drawing::Size(1180, 720);

            cmbRole = gcnew ComboBox();
            cmbRole->Visible = false;

            // Header
            headerPanel = gcnew Panel();
            headerPanel->Dock = DockStyle::Top;
            headerPanel->Height = 92;
            headerPanel->BackColor = Color::White;
            headerPanel->Padding =
                System::Windows::Forms::Padding(28, 18, 28, 12);

            lblTitle = gcnew Label();
            lblTitle->Text = L"Users & Roles";
            lblTitle->Dock = DockStyle::Top;
            lblTitle->Height = 34;
            lblTitle->Font =
                gcnew Drawing::Font(
                    L"Segoe UI Semibold",
                    18.0F
                );
            lblTitle->ForeColor =
                Color::FromArgb(15, 23, 42);

            lblSubtitle = gcnew Label();
            lblSubtitle->Text =
                L"Manage system accounts, role assignments and access permissions.";
            lblSubtitle->Dock = DockStyle::Top;
            lblSubtitle->Height = 25;
            lblSubtitle->Font =
                gcnew Drawing::Font(
                    L"Segoe UI",
                    9.5F
                );
            lblSubtitle->ForeColor =
                Color::FromArgb(71, 85, 105);

            headerPanel->Controls->Add(lblSubtitle);
            headerPanel->Controls->Add(lblTitle);

            // Toolbar
            toolbarPanel = gcnew Panel();
            toolbarPanel->Dock = DockStyle::Top;
            toolbarPanel->Height = 66;
            toolbarPanel->BackColor = Color::White;
            toolbarPanel->Padding =
                System::Windows::Forms::Padding(28, 12, 28, 12);

            FlowLayoutPanel^ actions =
                gcnew FlowLayoutPanel();

            actions->Dock = DockStyle::Left;
            actions->AutoSize = true;
            actions->WrapContents = false;

            btnAddUser = gcnew Button();
            btnAddUser->Text = L"Add User";
            btnAddUser->Width = 105;
            StyleButton(
                btnAddUser,
                Color::FromArgb(15, 118, 110)
            );
            btnAddUser->Click +=
                gcnew EventHandler(
                    this,
                    &UsersRoles::btnAddUser_Click
                );

            btnEditUser = gcnew Button();
            btnEditUser->Text = L"Edit User";
            btnEditUser->Width = 105;
            StyleButton(
                btnEditUser,
                Color::FromArgb(37, 99, 235)
            );
            btnEditUser->Click +=
                gcnew EventHandler(
                    this,
                    &UsersRoles::btnEditUser_Click
                );

            btnToggleUser = gcnew Button();
            btnToggleUser->Text = L"Activate / Deactivate";
            btnToggleUser->Width = 150;
            StyleButton(
                btnToggleUser,
                Color::FromArgb(71, 85, 105)
            );
            btnToggleUser->Click +=
                gcnew EventHandler(
                    this,
                    &UsersRoles::btnToggleUser_Click
                );

            btnResetPassword = gcnew Button();
            btnResetPassword->Text = L"Reset Password";
            btnResetPassword->Width = 125;
            StyleButton(
                btnResetPassword,
                Color::FromArgb(180, 83, 9)
            );
            btnResetPassword->Click +=
                gcnew EventHandler(
                    this,
                    &UsersRoles::btnResetPassword_Click
                );

            actions->Controls->Add(btnAddUser);
            actions->Controls->Add(btnEditUser);
            actions->Controls->Add(btnToggleUser);
            actions->Controls->Add(btnResetPassword);

            FlowLayoutPanel^ searchPanel =
                gcnew FlowLayoutPanel();

            searchPanel->Dock = DockStyle::Right;
            searchPanel->AutoSize = true;
            searchPanel->WrapContents = false;

            lblSearch = gcnew Label();
            lblSearch->Text = L"Search";
            lblSearch->Width = 55;
            lblSearch->Height = 32;
            lblSearch->TextAlign =
                ContentAlignment::MiddleLeft;

            txtSearch = gcnew TextBox();
            txtSearch->Width = 280;
            txtSearch->Height = 32;

            btnSearch = gcnew Button();
            btnSearch->Text = L"Search";
            btnSearch->Width = 80;
            StyleButton(
                btnSearch,
                Color::FromArgb(51, 65, 85)
            );
            btnSearch->Click +=
                gcnew EventHandler(
                    this,
                    &UsersRoles::btnSearch_Click
                );

            txtSearch->KeyDown +=
                gcnew KeyEventHandler(
                    this,
                    &UsersRoles::txtSearch_KeyDown
                );

            searchPanel->Controls->Add(lblSearch);
            searchPanel->Controls->Add(txtSearch);
            searchPanel->Controls->Add(btnSearch);

            toolbarPanel->Controls->Add(searchPanel);
            toolbarPanel->Controls->Add(actions);

            // Grid
            Panel^ gridPanel = gcnew Panel();
            gridPanel->Dock = DockStyle::Fill;
            gridPanel->Padding =
                System::Windows::Forms::Padding(28, 12, 14, 20);
            gridPanel->BackColor =
                Color::FromArgb(241, 245, 249);

            lblUserCount = gcnew Label();
            lblUserCount->Text = L"Users: 0";
            lblUserCount->Dock = DockStyle::Top;
            lblUserCount->Height = 28;
            lblUserCount->Font =
                gcnew Drawing::Font(
                    L"Segoe UI Semibold",
                    9.5F
                );
            lblUserCount->ForeColor =
                Color::FromArgb(71, 85, 105);

            usersGrid = gcnew DataGridView();
            usersGrid->Name = L"usersGrid";
            usersGrid->Dock = DockStyle::Fill;
            usersGrid->AllowUserToAddRows = false;
            usersGrid->AllowUserToDeleteRows = false;
            usersGrid->ReadOnly = true;
            usersGrid->MultiSelect = false;
            usersGrid->SelectionMode =
                DataGridViewSelectionMode::FullRowSelect;
            usersGrid->AutoGenerateColumns = false;
            usersGrid->BackgroundColor = Color::White;
            usersGrid->BorderStyle =
                BorderStyle::None;
            usersGrid->RowHeadersVisible = false;
            usersGrid->AutoSizeRowsMode =
                DataGridViewAutoSizeRowsMode::None;

            DataGridViewTextBoxColumn^ idColumn =
                gcnew DataGridViewTextBoxColumn();
            idColumn->Name = L"UserId";
            idColumn->Visible = false;

            DataGridViewTextBoxColumn^ usernameColumn =
                gcnew DataGridViewTextBoxColumn();
            usernameColumn->Name = L"Username";
            usernameColumn->HeaderText = L"Username";
            usernameColumn->Width = 145;

            DataGridViewTextBoxColumn^ nameColumn =
                gcnew DataGridViewTextBoxColumn();
            nameColumn->Name = L"FullName";
            nameColumn->HeaderText = L"Full Name";
            nameColumn->AutoSizeMode =
                DataGridViewAutoSizeColumnMode::Fill;
            nameColumn->FillWeight = 145;

            DataGridViewTextBoxColumn^ emailColumn =
                gcnew DataGridViewTextBoxColumn();
            emailColumn->Name = L"Email";
            emailColumn->HeaderText = L"Email";
            emailColumn->AutoSizeMode =
                DataGridViewAutoSizeColumnMode::Fill;
            emailColumn->FillWeight = 155;

            DataGridViewTextBoxColumn^ roleColumn =
                gcnew DataGridViewTextBoxColumn();
            roleColumn->Name = L"Role";
            roleColumn->HeaderText = L"Role";
            roleColumn->Width = 160;

            DataGridViewTextBoxColumn^ statusColumn =
                gcnew DataGridViewTextBoxColumn();
            statusColumn->Name = L"Status";
            statusColumn->HeaderText = L"Status";
            statusColumn->Width = 105;

            DataGridViewTextBoxColumn^ passwordColumn =
                gcnew DataGridViewTextBoxColumn();
            passwordColumn->Name = L"PasswordStatus";
            passwordColumn->HeaderText = L"Password";
            passwordColumn->Width = 110;

            usersGrid->Columns->Add(idColumn);
            usersGrid->Columns->Add(usernameColumn);
            usersGrid->Columns->Add(nameColumn);
            usersGrid->Columns->Add(emailColumn);
            usersGrid->Columns->Add(roleColumn);
            usersGrid->Columns->Add(statusColumn);
            usersGrid->Columns->Add(passwordColumn);

            usersGrid->ColumnHeadersHeight = 42;
            usersGrid->ColumnHeadersDefaultCellStyle =
                gcnew DataGridViewCellStyle();
            usersGrid->ColumnHeadersDefaultCellStyle->BackColor =
                Color::FromArgb(226, 232, 240);
            usersGrid->ColumnHeadersDefaultCellStyle->ForeColor =
                Color::FromArgb(30, 41, 59);
            usersGrid->ColumnHeadersDefaultCellStyle->Font =
                gcnew Drawing::Font(
                    L"Segoe UI Semibold",
                    9.0F
                );
            usersGrid->DefaultCellStyle =
                gcnew DataGridViewCellStyle();
            usersGrid->DefaultCellStyle->Font =
                gcnew Drawing::Font(
                    L"Segoe UI",
                    9.0F
                );
            usersGrid->DefaultCellStyle->ForeColor =
                Color::FromArgb(30, 41, 59);
            usersGrid->DefaultCellStyle->SelectionBackColor =
                Color::FromArgb(219, 234, 254);
            usersGrid->DefaultCellStyle->SelectionForeColor =
                Color::FromArgb(15, 23, 42);
            usersGrid->RowTemplate->Height = 38;

            usersGrid->SelectionChanged +=
                gcnew EventHandler(
                    this,
                    &UsersRoles::usersGrid_SelectionChanged
                );

            // Role panel
            rolePanel = gcnew Panel();
            rolePanel->Dock = DockStyle::Right;
            rolePanel->Width = 310;
            rolePanel->BackColor = Color::White;
            rolePanel->Padding =
                System::Windows::Forms::Padding(20, 18, 20, 18);

            lblRoleTitle = gcnew Label();
            lblRoleTitle->Text = L"Role & Permissions";
            lblRoleTitle->Dock = DockStyle::Top;
            lblRoleTitle->Height = 30;
            lblRoleTitle->Font =
                gcnew Drawing::Font(
                    L"Segoe UI Semibold",
                    12.0F
                );
            lblRoleTitle->ForeColor =
                Color::FromArgb(15, 23, 42);

            lblRoleName = gcnew Label();
            lblRoleName->Text = L"No role selected";
            lblRoleName->Dock = DockStyle::Top;
            lblRoleName->Height = 30;
            lblRoleName->Font =
                gcnew Drawing::Font(
                    L"Segoe UI Semibold",
                    10.5F
                );
            lblRoleName->ForeColor =
                Color::FromArgb(15, 118, 110);

            lblRoleDescription = gcnew Label();
            lblRoleDescription->Text = L"";
            lblRoleDescription->Dock = DockStyle::Top;
            lblRoleDescription->Height = 48;
            lblRoleDescription->AutoEllipsis = true;
            lblRoleDescription->ForeColor =
                Color::FromArgb(71, 85, 105);

            lblPermissionsTitle = gcnew Label();
            lblPermissionsTitle->Text = L"Assigned permissions";
            lblPermissionsTitle->Dock = DockStyle::Top;
            lblPermissionsTitle->Height = 30;
            lblPermissionsTitle->Font =
                gcnew Drawing::Font(
                    L"Segoe UI Semibold",
                    9.5F
                );

            lstPermissions = gcnew ListBox();
            lstPermissions->Dock = DockStyle::Fill;
            lstPermissions->BorderStyle =
                BorderStyle::FixedSingle;
            lstPermissions->Font =
                gcnew Drawing::Font(
                    L"Segoe UI",
                    9.0F
                );

            btnClose = gcnew Button();
            btnClose->Text = L"Close";
            btnClose->Dock = DockStyle::Bottom;
            btnClose->Height = 38;
            StyleButton(
                btnClose,
                Color::FromArgb(71, 85, 105)
            );
            btnClose->Click +=
                gcnew EventHandler(
                    this,
                    &UsersRoles::btnClose_Click
                );

            rolePanel->Controls->Add(lstPermissions);
            rolePanel->Controls->Add(lblPermissionsTitle);
            rolePanel->Controls->Add(lblRoleDescription);
            rolePanel->Controls->Add(lblRoleName);
            rolePanel->Controls->Add(lblRoleTitle);
            rolePanel->Controls->Add(btnClose);

            gridPanel->Controls->Add(usersGrid);
            gridPanel->Controls->Add(lblUserCount);

            this->Controls->Add(gridPanel);
            this->Controls->Add(rolePanel);
            this->Controls->Add(toolbarPanel);
            this->Controls->Add(headerPanel);

            this->ResumeLayout(false);
        }

        #pragma endregion
    };
}

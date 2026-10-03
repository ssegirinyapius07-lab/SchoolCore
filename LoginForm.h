#pragma once

#include "DbConnection.h"
#include "AuthSession.h"

#include <mariadb/conncpp.hpp>
#include <msclr/marshal_cppstd.h>

#include <memory>
#include <string>
#include <vector>

using namespace System;
using namespace System::Drawing;
using namespace System::Security::Cryptography;
using namespace System::Windows::Forms;
using namespace System::Collections::Generic;

namespace SchoolCore
{
    public ref class LoginForm : public Form
    {
    private:
        Panel^ card;
        Label^ lblTitle;
        Label^ lblSubtitle;
        Label^ lblUsername;
        Label^ lblPassword;
        TextBox^ txtUsername;
        TextBox^ txtPassword;
        Button^ btnLogin;
        Label^ lblMessage;
        CheckBox^ chkShowPassword;

        static array<Byte>^ Base64Decode(String^ value)
        {
            return Convert::FromBase64String(value);
        }

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

            array<Byte>^ hash =
                derive->GetBytes(32);

            delete derive;

            return Base64Encode(hash);
        }

        static bool SlowEquals(
            array<Byte>^ left,
            array<Byte>^ right)
        {
            if (left == nullptr || right == nullptr ||
                left->Length != right->Length)
            {
                return false;
            }

            int difference = 0;

            for (int i = 0; i < left->Length; i++)
            {
                difference |= left[i] ^ right[i];
            }

            return difference == 0;
        }

        static bool VerifyPassword(
            String^ password,
            String^ storedHash)
        {
            if (String::IsNullOrWhiteSpace(storedHash))
            {
                return false;
            }

            array<String^>^ parts =
                storedHash->Split(
                    gcnew array<wchar_t>{ L'$' }
                );

            if (parts->Length != 4 ||
                !parts[0]->Equals(
                    L"PBKDF2-SHA256",
                    StringComparison::Ordinal))
            {
                return false;
            }

            int iterations = 0;

            if (!Int32::TryParse(parts[1], iterations) ||
                iterations < 10000)
            {
                return false;
            }

            try
            {
                array<Byte>^ salt =
                    Base64Decode(parts[2]);

                array<Byte>^ expected =
                    Base64Decode(parts[3]);

                String^ actualText =
                    HashPassword(
                        password,
                        salt,
                        iterations
                    );

                array<Byte>^ actual =
                    Base64Decode(actualText);

                return SlowEquals(
                    actual,
                    expected
                );
            }
            catch (Exception^)
            {
                return false;
            }
        }

        void SetMessage(
            String^ message,
            bool error)
        {
            this->lblMessage->Text =
                message;

            this->lblMessage->ForeColor =
                error
                ? Color::Firebrick
                : Color::DarkGreen;
        }

        System::Void chkShowPassword_CheckedChanged(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            this->txtPassword->UseSystemPasswordChar =
                !this->chkShowPassword->Checked;
        }

        System::Void btnLogin_Click(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            String^ username =
                this->txtUsername->Text->Trim();

            String^ password =
                this->txtPassword->Text;

            if (String::IsNullOrWhiteSpace(username) ||
                String::IsNullOrEmpty(password))
            {
                SetMessage(
                    L"Enter your username and password.",
                    true
                );

                return;
            }

            this->btnLogin->Enabled = false;
            SetMessage(
                L"Signing in...",
                false
            );

            try
            {
                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> userStmt(
                    con->prepareStatement(
                        "SELECT "
                        "u.user_id, "
                        "u.username, "
                        "u.password_hash, "
                        "u.full_name, "
                        "u.role_id, "
                        "u.status, "
                        "u.must_change_password, "
                        "r.role_name "
                        "FROM users u "
                        "INNER JOIN roles r "
                        "ON r.role_id = u.role_id "
                        "WHERE u.username = ? "
                        "LIMIT 1"
                    )
                );

                userStmt->setString(
                    1,
                    msclr::interop::marshal_as<std::string>(
                        username
                    )
                );

                std::unique_ptr<sql::ResultSet> userResult(
                    userStmt->executeQuery()
                );

                if (!userResult->next())
                {
                    SetMessage(
                        L"Invalid username or password.",
                        true
                    );

                    this->btnLogin->Enabled = true;
                    this->txtPassword->SelectAll();
                    this->txtPassword->Focus();
                    return;
                }

                std::string dbStatus =
                    userResult->getString("status");

                if (dbStatus != "Active")
                {
                    SetMessage(
                        L"This account is inactive.",
                        true
                    );

                    this->btnLogin->Enabled = true;
                    return;
                }

                String^ storedHash =
                    gcnew String(
                        userResult->getString(
                            "password_hash"
                        ).c_str()
                    );

                if (!VerifyPassword(
                    password,
                    storedHash))
                {
                    SetMessage(
                        L"Invalid username or password.",
                        true
                    );

                    this->btnLogin->Enabled = true;
                    this->txtPassword->SelectAll();
                    this->txtPassword->Focus();
                    return;
                }

                int userId =
                    userResult->getInt("user_id");

                String^ fullName =
                    gcnew String(
                        userResult->getString(
                            "full_name"
                        ).c_str()
                    );

                String^ roleName =
                    gcnew String(
                        userResult->getString(
                            "role_name"
                        ).c_str()
                    );

                std::unique_ptr<sql::PreparedStatement> permissionStmt(
                    con->prepareStatement(
                        "SELECT p.permission_name "
                        "FROM role_permissions rp "
                        "INNER JOIN permissions p "
                        "ON p.permission_id = rp.permission_id "
                        "WHERE rp.role_id = ? "
                        "ORDER BY p.permission_id"
                    )
                );

                permissionStmt->setInt(
                    1,
                    userResult->getInt("role_id")
                );

                std::unique_ptr<sql::ResultSet> permissionResult(
                    permissionStmt->executeQuery()
                );

                List<String^>^ permissionList =
                    gcnew List<String^>();

                while (permissionResult->next())
                {
                    permissionList->Add(
                        gcnew String(
                            permissionResult->getString(
                                "permission_name"
                            ).c_str()
                        )
                    );
                }

                AuthSession::Start(
                    userId,
                    username,
                    fullName,
                    roleName,
                    permissionList
                );

                this->DialogResult =
                    System::Windows::Forms::DialogResult::OK;

                this->Close();
            }
            catch (sql::SQLException& ex)
            {
                SetMessage(
                    gcnew String(ex.what()),
                    true
                );

                this->btnLogin->Enabled = true;
            }
            catch (Exception^ ex)
            {
                SetMessage(
                    ex->Message,
                    true
                );

                this->btnLogin->Enabled = true;
            }
        }

        void InitializeComponent()
        {
            this->SuspendLayout();

            this->Text = L"SchoolCore Login";
            this->StartPosition =
                FormStartPosition::CenterScreen;
            this->FormBorderStyle =
                System::Windows::Forms::FormBorderStyle::FixedSingle;
            this->MaximizeBox = false;
            this->MinimizeBox = false;
            this->ClientSize =
                System::Drawing::Size(
                    460,
                    470
                );
            this->BackColor =
                Color::FromArgb(
                    241,
                    245,
                    249
                );
            this->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    10.0F
                );

            this->card =
                gcnew Panel();

            this->card->Size =
                System::Drawing::Size(
                    390,
                    400
                );

            this->card->Location =
                Point(35, 35);

            this->card->BackColor =
                Color::White;

            this->card->BorderStyle =
                BorderStyle::FixedSingle;

            this->lblTitle =
                gcnew Label();

            this->lblTitle->Text =
                L"SchoolCore";

            this->lblTitle->AutoSize = true;
            this->lblTitle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    24.0F,
                    FontStyle::Bold
                );
            this->lblTitle->ForeColor =
                Color::FromArgb(
                    30,
                    41,
                    59
                );
            this->lblTitle->Location =
                Point(35, 28);

            this->lblSubtitle =
                gcnew Label();

            this->lblSubtitle->Text =
                L"Sign in to the Secondary School Management System";
            this->lblSubtitle->Size =
                System::Drawing::Size(
                    320,
                    48
                );
            this->lblSubtitle->Location =
                Point(35, 72);
            this->lblSubtitle->ForeColor =
                Color::DimGray;

            this->lblUsername =
                gcnew Label();

            this->lblUsername->Text =
                L"Username";
            this->lblUsername->AutoSize = true;
            this->lblUsername->Location =
                Point(35, 135);

            this->txtUsername =
                gcnew TextBox();

            this->txtUsername->Size =
                System::Drawing::Size(
                    320,
                    34
                );
            this->txtUsername->Location =
                Point(35, 158);

            this->lblPassword =
                gcnew Label();

            this->lblPassword->Text =
                L"Password";
            this->lblPassword->AutoSize = true;
            this->lblPassword->Location =
                Point(35, 205);

            this->txtPassword =
                gcnew TextBox();

            this->txtPassword->Size =
                System::Drawing::Size(
                    320,
                    34
                );
            this->txtPassword->Location =
                Point(35, 228);
            this->txtPassword->UseSystemPasswordChar =
                true;

            this->chkShowPassword =
                gcnew CheckBox();

            this->chkShowPassword->Text =
                L"Show password";
            this->chkShowPassword->AutoSize = true;
            this->chkShowPassword->Location =
                Point(35, 269);

            this->btnLogin =
                gcnew Button();

            this->btnLogin->Text =
                L"Sign In";
            this->btnLogin->Size =
                System::Drawing::Size(
                    320,
                    42
                );
            this->btnLogin->Location =
                Point(35, 305);
            this->btnLogin->BackColor =
                Color::FromArgb(
                    30,
                    41,
                    59
                );
            this->btnLogin->ForeColor =
                Color::White;
            this->btnLogin->FlatStyle =
                FlatStyle::Flat;
            this->btnLogin->FlatAppearance->BorderSize =
                0;

            this->lblMessage =
                gcnew Label();

            this->lblMessage->Text =
                L"";
            this->lblMessage->Size =
                System::Drawing::Size(
                    320,
                    38
                );
            this->lblMessage->Location =
                Point(35, 352);
            this->lblMessage->TextAlign =
                ContentAlignment::MiddleLeft;

            this->card->Controls->Add(
                this->lblMessage
            );
            this->card->Controls->Add(
                this->btnLogin
            );
            this->card->Controls->Add(
                this->chkShowPassword
            );
            this->card->Controls->Add(
                this->txtPassword
            );
            this->card->Controls->Add(
                this->lblPassword
            );
            this->card->Controls->Add(
                this->txtUsername
            );
            this->card->Controls->Add(
                this->lblUsername
            );
            this->card->Controls->Add(
                this->lblSubtitle
            );
            this->card->Controls->Add(
                this->lblTitle
            );

            this->Controls->Add(
                this->card
            );

            this->btnLogin->Click +=
                gcnew EventHandler(
                    this,
                    &LoginForm::btnLogin_Click
                );

            this->chkShowPassword->CheckedChanged +=
                gcnew EventHandler(
                    this,
                    &LoginForm::chkShowPassword_CheckedChanged
                );

            this->AcceptButton =
                this->btnLogin;

            this->ResumeLayout(false);
        }

    public:
        LoginForm()
        {
            InitializeComponent();
        }
    };
}

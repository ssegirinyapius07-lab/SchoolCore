#pragma once

#include "DbConnection.h"
#include "ThemeManager.h"

#include <mariadb/conncpp.hpp>
#include <msclr/marshal_cppstd.h>

#include <memory>
#include <string>

namespace SchoolCore
{
    using namespace System;
    using namespace System::Drawing;
    using namespace System::Security::Cryptography;
    using namespace System::Windows::Forms;

    public ref class PasswordChangeForm : public Form
    {
    public:
        PasswordChangeForm(int userId, String^ username)
        {
            this->userId = userId;
            this->username = username;
            InitializeComponent();
            ThemeManager::ApplyToForm(this);
        }

        System::ComponentModel::Container^ components;

    protected:
        ~PasswordChangeForm(void)
        {
            if (components)
            {
                delete components;
            }
        }

    private:
        int userId;
        String^ username;

        Panel^ card;
        Label^ lblTitle;
        Label^ lblSubtitle;
        Label^ lblUsername;
        Label^ lblNewPassword;
        Label^ lblConfirmPassword;
        Label^ lblMessage;
        TextBox^ txtNewPassword;
        TextBox^ txtConfirmPassword;
        Button^ btnChange;
        Button^ btnCancel;
        CheckBox^ chkShowPassword;

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

        static String^ CreatePasswordHash(String^ password)
        {
            const int iterations = 100000;

            array<Byte>^ salt =
                gcnew array<Byte>(16);

            RandomNumberGenerator^ rng =
                RandomNumberGenerator::Create();

            rng->GetBytes(salt);
            delete rng;

            return L"PBKDF2-SHA256$" +
                iterations.ToString() +
                L"$" +
                Base64Encode(salt) +
                L"$" +
                HashPassword(
                    password,
                    salt,
                    iterations
                );
        }

        void SetMessage(
            String^ message,
            bool error)
        {
            lblMessage->Text = message;
            lblMessage->ForeColor =
                error
                ? ThemeManager::Danger()
                : ThemeManager::Success();
        }

        bool IsStrongEnough(String^ password)
        {
            if (password == nullptr ||
                password->Length < 8)
            {
                return false;
            }

            bool hasLetter = false;
            bool hasDigit = false;

            for each (wchar_t c in password)
            {
                if (Char::IsLetter(c))
                {
                    hasLetter = true;
                }

                if (Char::IsDigit(c))
                {
                    hasDigit = true;
                }
            }

            return hasLetter && hasDigit;
        }

        System::Void btnChange_Click(
            Object^ sender,
            EventArgs^ e)
        {
            String^ password =
                txtNewPassword->Text;

            String^ confirm =
                txtConfirmPassword->Text;

            if (!IsStrongEnough(password))
            {
                SetMessage(
                    L"Use at least 8 characters with letters and numbers.",
                    true
                );
                txtNewPassword->Focus();
                return;
            }

            if (password != confirm)
            {
                SetMessage(
                    L"The passwords do not match.",
                    true
                );
                txtConfirmPassword->SelectAll();
                txtConfirmPassword->Focus();
                return;
            }

            try
            {
                String^ hash =
                    CreatePasswordHash(password);

                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "UPDATE users "
                        "SET password_hash = ?, "
                        "must_change_password = 0, "
                        "password_changed_at = NOW() "
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

                SetMessage(
                    L"Password changed successfully.",
                    false
                );

                DialogResult =
                    System::Windows::Forms::DialogResult::OK;

                Close();
            }
            catch (sql::SQLException& ex)
            {
                System::Diagnostics::Debug::WriteLine(
                    gcnew String(ex.what())
                );

                SetMessage(
                    L"Unable to change the password right now. Please try again.",
                    true
                );
            }
            catch (Exception^ ex)
            {
                System::Diagnostics::Debug::WriteLine(
                    ex->Message
                );

                SetMessage(
                    L"Unable to change the password right now. Please try again.",
                    true
                );
            }
        }

        System::Void btnCancel_Click(
            Object^ sender,
            EventArgs^ e)
        {
            DialogResult =
                System::Windows::Forms::DialogResult::Cancel;

            Close();
        }

        System::Void chkShowPassword_CheckedChanged(
            Object^ sender,
            EventArgs^ e)
        {
            bool show =
                chkShowPassword->Checked;

            txtNewPassword->UseSystemPasswordChar =
                !show;

            txtConfirmPassword->UseSystemPasswordChar =
                !show;
        }

        System::Void Field_Enter(
            Object^ sender,
            EventArgs^ e)
        {
            Control^ box = safe_cast<Control^>(sender);

            if (box->Parent != nullptr &&
                box->Parent->Parent != nullptr)
            {
                box->Parent->Parent->BackColor =
                    ThemeManager::Accent();
            }
        }

        System::Void Field_Leave(
            Object^ sender,
            EventArgs^ e)
        {
            Control^ box = safe_cast<Control^>(sender);

            if (box->Parent != nullptr &&
                box->Parent->Parent != nullptr)
            {
                box->Parent->Parent->BackColor =
                    ThemeManager::Border();
            }
        }

        // Builds a bordered input: outer panel draws the border,
        // inner panel holds the borderless text box.
        Panel^ BuildField(
            TextBox^ box,
            int left,
            int top,
            int width,
            int tabIndex)
        {
            Panel^ outer = gcnew Panel();
            outer->Size = System::Drawing::Size(width, 44);
            outer->Location = System::Drawing::Point(left, top);
            outer->BackColor = ThemeManager::Border();
            outer->Padding = System::Windows::Forms::Padding(1);
            outer->TabIndex = tabIndex;

            Panel^ inner = gcnew Panel();
            inner->Dock = DockStyle::Fill;
            inner->BackColor = ThemeManager::Surface();
            inner->Padding =
                System::Windows::Forms::Padding(12, 11, 12, 0);

            box->Dock = DockStyle::Fill;
            box->BorderStyle =
                System::Windows::Forms::BorderStyle::None;
            box->BackColor = ThemeManager::Surface();
            box->ForeColor = ThemeManager::Ink();
            box->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    10.5F
                );
            box->UseSystemPasswordChar = true;
            box->TabIndex = 0;

            box->Enter +=
                gcnew EventHandler(
                    this,
                    &PasswordChangeForm::Field_Enter
                );

            box->Leave +=
                gcnew EventHandler(
                    this,
                    &PasswordChangeForm::Field_Leave
                );

            inner->Controls->Add(box);
            outer->Controls->Add(inner);

            return outer;
        }

        Label^ BuildCaption(
            String^ text,
            int left,
            int top)
        {
            Label^ caption = gcnew Label();
            caption->Text = text;
            caption->AutoSize = true;
            caption->Location = System::Drawing::Point(left, top);
            caption->ForeColor = ThemeManager::TextSecondary();
            caption->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    9.0F
                );

            return caption;
        }

        #pragma region Windows Form Designer generated code

        void InitializeComponent(void)
        {
            components =
                gcnew System::ComponentModel::Container();

            SuspendLayout();

            Text =
                L"SchoolCore | Change Password";

            StartPosition =
                FormStartPosition::CenterParent;

            FormBorderStyle =
                System::Windows::Forms::FormBorderStyle::FixedDialog;

            MaximizeBox = false;
            MinimizeBox = false;

            ClientSize =
                System::Drawing::Size(440, 480);

            BackColor = ThemeManager::Surface();
            ForeColor = ThemeManager::Ink();

            Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    10.0F
                );

            card = gcnew Panel();
            card->Dock = DockStyle::Fill;
            card->BackColor = ThemeManager::Surface();

            lblTitle = gcnew Label();
            lblTitle->Text = L"Set your password";
            lblTitle->AutoSize = true;
            lblTitle->Location = System::Drawing::Point(34, 30);
            lblTitle->ForeColor = ThemeManager::Ink();
            lblTitle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    18.0F
                );

            lblSubtitle = gcnew Label();
            lblSubtitle->Text =
                L"Your temporary password must be replaced before you continue.";
            lblSubtitle->Size = System::Drawing::Size(370, 40);
            lblSubtitle->Location = System::Drawing::Point(36, 74);
            lblSubtitle->ForeColor = ThemeManager::TextMuted();
            lblSubtitle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.5F
                );

            lblUsername = gcnew Label();
            lblUsername->Text = L"Account: " + username;
            lblUsername->AutoSize = true;
            lblUsername->Location = System::Drawing::Point(36, 120);
            lblUsername->ForeColor = ThemeManager::TextSecondary();
            lblUsername->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    9.5F
                );

            lblNewPassword =
                BuildCaption(L"New password", 36, 158);

            txtNewPassword = gcnew TextBox();

            Panel^ newField =
                BuildField(txtNewPassword, 36, 182, 368, 0);

            lblConfirmPassword =
                BuildCaption(L"Confirm password", 36, 238);

            txtConfirmPassword = gcnew TextBox();

            Panel^ confirmField =
                BuildField(txtConfirmPassword, 36, 262, 368, 1);

            chkShowPassword = gcnew CheckBox();
            chkShowPassword->Text = L"Show passwords";
            chkShowPassword->AutoSize = true;
            chkShowPassword->Location =
                System::Drawing::Point(36, 316);
            chkShowPassword->ForeColor =
                ThemeManager::TextSecondary();
            chkShowPassword->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.0F
                );
            chkShowPassword->TabIndex = 2;
            chkShowPassword->CheckedChanged +=
                gcnew EventHandler(
                    this,
                    &PasswordChangeForm::chkShowPassword_CheckedChanged
                );

            lblMessage = gcnew Label();
            lblMessage->Text = L"";
            lblMessage->Size = System::Drawing::Size(368, 44);
            lblMessage->Location = System::Drawing::Point(36, 348);
            lblMessage->ForeColor = ThemeManager::Danger();
            lblMessage->TextAlign =
                System::Drawing::ContentAlignment::MiddleLeft;
            lblMessage->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.0F
                );

            btnChange = gcnew Button();
            btnChange->Text = L"Change password";
            btnChange->Size = System::Drawing::Size(170, 42);
            btnChange->Location = System::Drawing::Point(234, 410);
            btnChange->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    9.5F
                );
            btnChange->TabIndex = 3;
            ThemeManager::StylePrimaryButton(btnChange);
            btnChange->Click +=
                gcnew EventHandler(
                    this,
                    &PasswordChangeForm::btnChange_Click
                );

            btnCancel = gcnew Button();
            btnCancel->Text = L"Cancel";
            btnCancel->Size = System::Drawing::Size(90, 42);
            btnCancel->Location = System::Drawing::Point(136, 410);
            btnCancel->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.5F
                );
            btnCancel->TabIndex = 4;
            ThemeManager::StyleSecondaryButton(btnCancel);
            btnCancel->Click +=
                gcnew EventHandler(
                    this,
                    &PasswordChangeForm::btnCancel_Click
                );

            card->Controls->Add(btnCancel);
            card->Controls->Add(btnChange);
            card->Controls->Add(lblMessage);
            card->Controls->Add(chkShowPassword);
            card->Controls->Add(confirmField);
            card->Controls->Add(lblConfirmPassword);
            card->Controls->Add(newField);
            card->Controls->Add(lblNewPassword);
            card->Controls->Add(lblUsername);
            card->Controls->Add(lblSubtitle);
            card->Controls->Add(lblTitle);

            Controls->Add(card);

            AcceptButton = btnChange;
            CancelButton = btnCancel;
            ActiveControl = txtNewPassword;

            ResumeLayout(false);
        }

        #pragma endregion
    };
}

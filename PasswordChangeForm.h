#pragma once

#include "DbConnection.h"

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
                ? Color::Firebrick
                : Color::DarkGreen;
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
                SetMessage(
                    gcnew String(ex.what()),
                    true
                );
            }
            catch (Exception^ ex)
            {
                SetMessage(
                    ex->Message,
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
                Drawing::Size(520, 430);

            BackColor =
                Color::FromArgb(
                    241,
                    245,
                    249
                );

            card = gcnew Panel();
            card->Dock = DockStyle::Fill;
            card->BackColor = Color::White;
            card->Padding =
                System::Windows::Forms::Padding(38, 30, 38, 30);

            lblTitle = gcnew Label();
            lblTitle->Text =
                L"Set Your Password";
            lblTitle->Dock = DockStyle::Top;
            lblTitle->Height = 38;
            lblTitle->Font =
                gcnew Drawing::Font(
                    L"Segoe UI Semibold",
                    18.0F
                );
            lblTitle->ForeColor =
                Color::FromArgb(15, 23, 42);

            lblSubtitle = gcnew Label();
            lblSubtitle->Text =
                L"Your temporary password must be replaced before continuing.";
            lblSubtitle->Dock = DockStyle::Top;
            lblSubtitle->Height = 44;
            lblSubtitle->ForeColor =
                Color::FromArgb(71, 85, 105);

            lblUsername = gcnew Label();
            lblUsername->Text =
                L"Account: " + username;
            lblUsername->Dock = DockStyle::Top;
            lblUsername->Height = 32;
            lblUsername->Font =
                gcnew Drawing::Font(
                    L"Segoe UI Semibold",
                    9.5F
                );
            lblUsername->ForeColor =
                Color::FromArgb(15, 118, 110);

            lblNewPassword = gcnew Label();
            lblNewPassword->Text =
                L"New password";
            lblNewPassword->Dock = DockStyle::Top;
            lblNewPassword->Height = 28;

            txtNewPassword = gcnew TextBox();
            txtNewPassword->Dock = DockStyle::Top;
            txtNewPassword->Height = 34;
            txtNewPassword->UseSystemPasswordChar = true;

            lblConfirmPassword = gcnew Label();
            lblConfirmPassword->Text =
                L"Confirm password";
            lblConfirmPassword->Dock = DockStyle::Top;
            lblConfirmPassword->Height = 28;
            lblConfirmPassword->Margin =
                System::Windows::Forms::Padding(0, 12, 0, 0);

            txtConfirmPassword = gcnew TextBox();
            txtConfirmPassword->Dock = DockStyle::Top;
            txtConfirmPassword->Height = 34;
            txtConfirmPassword->UseSystemPasswordChar = true;

            chkShowPassword = gcnew CheckBox();
            chkShowPassword->Text =
                L"Show passwords";
            chkShowPassword->Dock = DockStyle::Top;
            chkShowPassword->Height = 28;
            chkShowPassword->CheckedChanged +=
                gcnew EventHandler(
                    this,
                    &PasswordChangeForm::chkShowPassword_CheckedChanged
                );

            lblMessage = gcnew Label();
            lblMessage->Text = L"";
            lblMessage->Dock = DockStyle::Top;
            lblMessage->Height = 38;
            lblMessage->ForeColor =
                Color::Firebrick;
            lblMessage->AutoEllipsis = true;

            FlowLayoutPanel^ buttonPanel =
                gcnew FlowLayoutPanel();

            buttonPanel->Dock = DockStyle::Bottom;
            buttonPanel->Height = 48;
            buttonPanel->FlowDirection =
                FlowDirection::RightToLeft;
            buttonPanel->WrapContents = false;

            btnChange = gcnew Button();
            btnChange->Text =
                L"Change Password";
            btnChange->Width = 140;
            btnChange->Height = 36;
            btnChange->BackColor =
                Color::FromArgb(15, 118, 110);
            btnChange->ForeColor = Color::White;
            btnChange->FlatStyle =
                FlatStyle::Flat;
            btnChange->FlatAppearance->BorderSize = 0;
            btnChange->Font =
                gcnew Drawing::Font(
                    L"Segoe UI Semibold",
                    9.5F
                );
            btnChange->Click +=
                gcnew EventHandler(
                    this,
                    &PasswordChangeForm::btnChange_Click
                );

            btnCancel = gcnew Button();
            btnCancel->Text = L"Cancel";
            btnCancel->Width = 90;
            btnCancel->Height = 36;
            btnCancel->Click +=
                gcnew EventHandler(
                    this,
                    &PasswordChangeForm::btnCancel_Click
                );

            buttonPanel->Controls->Add(btnChange);
            buttonPanel->Controls->Add(btnCancel);

            card->Controls->Add(buttonPanel);
            card->Controls->Add(lblMessage);
            card->Controls->Add(chkShowPassword);
            card->Controls->Add(txtConfirmPassword);
            card->Controls->Add(lblConfirmPassword);
            card->Controls->Add(txtNewPassword);
            card->Controls->Add(lblNewPassword);
            card->Controls->Add(lblUsername);
            card->Controls->Add(lblSubtitle);
            card->Controls->Add(lblTitle);

            Controls->Add(card);

            AcceptButton = btnChange;
            CancelButton = btnCancel;

            ResumeLayout(false);
        }

        #pragma endregion
    };
}

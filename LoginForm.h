#pragma once

#include "DbConnection.h"
#include "AuthSession.h"

#include <mariadb/conncpp.hpp>
#include <msclr/marshal_cppstd.h>

#include <memory>
#include <string>
#include <vector>

namespace SchoolCore
{
    using namespace System;
    using namespace System::Drawing;
    using namespace System::Security::Cryptography;
    using namespace System::Windows::Forms;
    using namespace System::Collections::Generic;
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
        Button^ btnClose;

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

        System::Void btnClose_Click(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            this->DialogResult =
                System::Windows::Forms::DialogResult::Cancel;

            this->Close();
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

        void CenterLoginCard()
        {
            if (this->card == nullptr)
            {
                return;
            }

            int x =
                (this->ClientSize.Width -
                    this->card->Width) / 2;

            int y =
                (this->ClientSize.Height -
                    this->card->Height) / 2;

            if (x < 20)
            {
                x = 20;
            }

            if (y < 20)
            {
                y = 20;
            }

            this->card->Location =
                Point(x, y);
        }

        System::Void LoginForm_Resize(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            CenterLoginCard();
        }

        void InitializeComponent()
        {
            this->SuspendLayout();

            // =====================================================
            // FORM
            // =====================================================

            this->Text =
                L"SchoolCore | Sign In";

            this->StartPosition =
                FormStartPosition::CenterScreen;

            this->FormBorderStyle =
                System::Windows::Forms::FormBorderStyle::None;

            this->MaximizeBox = false;
            this->MinimizeBox = false;

            this->WindowState =
                FormWindowState::Maximized;

            this->BackColor =
                Color::FromArgb(
                    241,
                    245,
                    249
                );

            this->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    10.0F,
                    FontStyle::Regular
                );


            // =====================================================
            // MAIN CARD
            // =====================================================

            this->card =
                gcnew Panel();

            this->card->Size =
                System::Drawing::Size(
                    420,
                    520
                );

            this->card->Location =
                Point(
                    50,
                    40
                );

            this->card->BackColor =
                Color::White;

            this->card->BorderStyle =
                BorderStyle::FixedSingle;


            // =====================================================
            // CLOSE LOGIN
            // =====================================================

            this->btnClose =
                gcnew Button();

            this->btnClose->Text =
                L"\u00D7";

            this->btnClose->Size =
                System::Drawing::Size(
                    42,
                    42
                );

            this->btnClose->Location =
                Point(
                    this->ClientSize.Width - 58,
                    18
                );

            this->btnClose->Anchor =
                AnchorStyles::Top |
                AnchorStyles::Right;

            this->btnClose->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    18.0F,
                    FontStyle::Regular
                );

            this->btnClose->ForeColor =
                Color::FromArgb(
                    71,
                    85,
                    105
                );

            this->btnClose->BackColor =
                Color::Transparent;

            this->btnClose->FlatStyle =
                FlatStyle::Flat;

            this->btnClose->FlatAppearance->BorderSize =
                0;

            this->btnClose->Cursor =
                Cursors::Hand;


            // =====================================================
            // BRANDING
            // =====================================================

            Panel^ brandPanel =
                gcnew Panel();

            brandPanel->Dock =
                DockStyle::Top;

            brandPanel->Height =
                145;

            brandPanel->BackColor =
                Color::FromArgb(
                    30,
                    41,
                    59
                );


            Label^ brandMark =
                gcnew Label();

            brandMark->Text =
                L"SC";

            brandMark->Size =
                System::Drawing::Size(
                    58,
                    58
                );

            brandMark->Location =
                Point(
                    181,
                    18
                );

            brandMark->BackColor =
                Color::FromArgb(
                    59,
                    130,
                    246
                );

            brandMark->ForeColor =
                Color::White;

            brandMark->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    20.0F,
                    FontStyle::Bold
                );

            brandMark->TextAlign =
                ContentAlignment::MiddleCenter;


            this->lblTitle =
                gcnew Label();

            this->lblTitle->Text =
                L"SchoolCore";

            this->lblTitle->Dock =
                DockStyle::Bottom;

            this->lblTitle->Height =
                42;

            this->lblTitle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    22.0F,
                    FontStyle::Bold
                );

            this->lblTitle->ForeColor =
                Color::White;

            this->lblTitle->TextAlign =
                ContentAlignment::MiddleCenter;


            brandPanel->Controls->Add(
                this->lblTitle
            );

            brandPanel->Controls->Add(
                brandMark
            );


            // =====================================================
            // SUBTITLE
            // =====================================================

            this->lblSubtitle =
                gcnew Label();

            this->lblSubtitle->Text =
                L"Secondary School Management System";

            this->lblSubtitle->Dock =
                DockStyle::Top;

            this->lblSubtitle->Height =
                48;

            this->lblSubtitle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.5F,
                    FontStyle::Regular
                );

            this->lblSubtitle->ForeColor =
                Color::DimGray;

            this->lblSubtitle->TextAlign =
                ContentAlignment::MiddleCenter;

            this->lblSubtitle->Padding =
                System::Windows::Forms::Padding(
                    0,
                    12,
                    0,
                    0
                );


            // =====================================================
            // USERNAME
            // =====================================================

            this->lblUsername =
                gcnew Label();

            this->lblUsername->Text =
                L"USERNAME";

            this->lblUsername->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    8.5F,
                    FontStyle::Bold
                );

            this->lblUsername->ForeColor =
                Color::FromArgb(
                    71,
                    85,
                    105
                );

            this->lblUsername->Location =
                Point(
                    45,
                    215
                );

            this->lblUsername->AutoSize =
                true;


            this->txtUsername =
                gcnew TextBox();

            this->txtUsername->Location =
                Point(
                    45,
                    240
                );

            this->txtUsername->Size =
                System::Drawing::Size(
                    330,
                    34
                );

            this->txtUsername->BorderStyle =
                BorderStyle::FixedSingle;

            this->txtUsername->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    10.5F
                );


            // =====================================================
            // PASSWORD
            // =====================================================

            this->lblPassword =
                gcnew Label();

            this->lblPassword->Text =
                L"PASSWORD";

            this->lblPassword->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    8.5F,
                    FontStyle::Bold
                );

            this->lblPassword->ForeColor =
                Color::FromArgb(
                    71,
                    85,
                    105
                );

            this->lblPassword->Location =
                Point(
                    45,
                    292
                );

            this->lblPassword->AutoSize =
                true;


            this->txtPassword =
                gcnew TextBox();

            this->txtPassword->Location =
                Point(
                    45,
                    317
                );

            this->txtPassword->Size =
                System::Drawing::Size(
                    330,
                    34
                );

            this->txtPassword->BorderStyle =
                BorderStyle::FixedSingle;

            this->txtPassword->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    10.5F
                );

            this->txtPassword->UseSystemPasswordChar =
                true;


            // =====================================================
            // SHOW PASSWORD
            // =====================================================

            this->chkShowPassword =
                gcnew CheckBox();

            this->chkShowPassword->Text =
                L"Show password";

            this->chkShowPassword->AutoSize =
                true;

            this->chkShowPassword->Location =
                Point(
                    45,
                    360
                );

            this->chkShowPassword->ForeColor =
                Color::DimGray;


            // =====================================================
            // SIGN IN BUTTON
            // =====================================================

            this->btnLogin =
                gcnew Button();

            this->btnLogin->Text =
                L"Sign In";

            this->btnLogin->Size =
                System::Drawing::Size(
                    330,
                    44
                );

            this->btnLogin->Location =
                Point(
                    45,
                    394
                );

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

            this->btnLogin->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    10.5F,
                    FontStyle::Bold
                );

            this->btnLogin->Cursor =
                Cursors::Hand;


            // =====================================================
            // MESSAGE
            // =====================================================

            this->lblMessage =
                gcnew Label();

            this->lblMessage->Text =
                L"";

            this->lblMessage->Size =
                System::Drawing::Size(
                    330,
                    40
                );

            this->lblMessage->Location =
                Point(
                    45,
                    445
                );

            this->lblMessage->TextAlign =
                ContentAlignment::MiddleLeft;

            this->lblMessage->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.0F
                );


            // =====================================================
            // FOOTER
            // =====================================================

            Label^ footer =
                gcnew Label();

            footer->Text =
                L"Secure access | SchoolCore";

            footer->Dock =
                DockStyle::Bottom;

            footer->Height =
                32;

            footer->ForeColor =
                Color::Gray;

            footer->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    8.5F
                );

            footer->TextAlign =
                ContentAlignment::MiddleCenter;


            // =====================================================
            // ADD CONTROLS
            // =====================================================

            this->card->Controls->Add(
                footer
            );

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
                brandPanel
            );


            this->Controls->Add(
                this->card
            );

            this->Controls->Add(
                this->btnClose
            );


            // =====================================================
            // EVENTS
            // =====================================================

            this->btnLogin->Click +=
                gcnew EventHandler(
                    this,
                    &LoginForm::btnLogin_Click
                );

            this->btnClose->Click +=
                gcnew EventHandler(
                    this,
                    &LoginForm::btnClose_Click
                );

            this->chkShowPassword->CheckedChanged +=
                gcnew EventHandler(
                    this,
                    &LoginForm::chkShowPassword_CheckedChanged
                );


            this->AcceptButton =
                this->btnLogin;

            this->CancelButton =
                this->btnClose;

            this->Resize +=
                gcnew EventHandler(
                    this,
                    &LoginForm::LoginForm_Resize
                );

            this->ResumeLayout(false);

            CenterLoginCard();
        }

    public:
        LoginForm()
        {
            InitializeComponent();
        }
    };
}

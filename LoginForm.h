#pragma once

#include "DbConnection.h"
#include "ThemeManager.h"
#include "AuthSession.h"
#include "PasswordChangeForm.h"

#include <mariadb/conncpp.hpp>
#include <msclr/marshal_cppstd.h>

#include <memory>
#include <string>
#include <vector>

namespace SchoolCore
{
    using namespace System;
    using namespace System::ComponentModel;
using namespace System::Drawing;
    using namespace System::Security::Cryptography;
    using namespace System::Windows::Forms;
    using namespace System::Collections::Generic;
    public ref class LoginForm : public System::Windows::Forms::Form
    {
    public:
                LoginForm(void)
                {
                    InitializeComponent();
                                    ThemeManager::ApplyToForm(this);
                    }

        System::ComponentModel::Container^ components;


    protected:
        ~LoginForm(void)
        {
            if (this->components)
            {
                delete this->components;
            }
        }

    private:
        System::Windows::Forms::Panel^ card;
        System::Windows::Forms::Label^ lblTitle;
        System::Windows::Forms::Label^ lblSubtitle;
        System::Windows::Forms::Label^ lblUsername;
        System::Windows::Forms::Label^ lblPassword;
        System::Windows::Forms::TextBox^ txtUsername;
        System::Windows::Forms::TextBox^ txtPassword;
        System::Windows::Forms::Button^ btnLogin;
        System::Windows::Forms::Label^ lblMessage;
        System::Windows::Forms::CheckBox^ chkShowPassword;
        System::Windows::Forms::Button^ btnClose;
        System::Windows::Forms::Panel^ userField;
        System::Windows::Forms::Panel^ passField;

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
                ? ThemeManager::Danger()
                : ThemeManager::Success();
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

                bool mustChangePassword =
                    userResult->getBoolean(
                        "must_change_password"
                    );

                AuthSession::Start(
                    userId,
                    username,
                    fullName,
                    roleName,
                    permissionList
                );

                if (mustChangePassword)
                {
                    PasswordChangeForm^ passwordForm =
                        gcnew PasswordChangeForm(
                            userId,
                            username
                        );

                    System::Windows::Forms::DialogResult passwordResult =
                        passwordForm->ShowDialog(this);

                    delete passwordForm;

                    if (passwordResult !=
                        System::Windows::Forms::DialogResult::OK)
                    {
                        AuthSession::Clear();

                        SetMessage(
                            L"Password change is required before signing in.",
                            true
                        );

                        this->btnLogin->Enabled = true;
                        return;
                    }
                }

                this->DialogResult =
                    System::Windows::Forms::DialogResult::OK;

                this->Close();
            }
            catch (sql::SQLException& ex)
            {
                System::Diagnostics::Debug::WriteLine(
                    gcnew String(ex.what())
                );

                SetMessage(
                    L"Unable to sign in right now. Please try again.",
                    true
                );

                this->btnLogin->Enabled = true;
            }
            catch (Exception^ ex)
            {
                System::Diagnostics::Debug::WriteLine(
                    ex->Message
                );

                SetMessage(
                    L"Unable to sign in right now. Please try again.",
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

        System::Void Field_Enter(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            System::Windows::Forms::Control^ box =
                safe_cast<System::Windows::Forms::Control^>(sender);

            if (box->Parent != nullptr &&
                box->Parent->Parent != nullptr)
            {
                box->Parent->Parent->BackColor =
                    ThemeManager::Accent();
            }
        }

        System::Void Field_Leave(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            System::Windows::Forms::Control^ box =
                safe_cast<System::Windows::Forms::Control^>(sender);

            if (box->Parent != nullptr &&
                box->Parent->Parent != nullptr)
            {
                box->Parent->Parent->BackColor =
                    ThemeManager::Border();
            }
        }

        System::Void card_Paint(
            System::Object^ sender,
            System::Windows::Forms::PaintEventArgs^ e)
        {
            System::Drawing::Pen^ pen =
                gcnew System::Drawing::Pen(
                    ThemeManager::Divider()
                );

            e->Graphics->DrawRectangle(
                pen,
                0,
                0,
                this->card->Width - 1,
                this->card->Height - 1
            );

            delete pen;
        }

        #pragma region Windows Form Designer generated code

        void InitializeComponent(void)
        {
            this->components = gcnew System::ComponentModel::Container();
            this->SuspendLayout();

            // =====================================================
            // FORM
            // =====================================================

            this->Text = L"SchoolCore | Sign In";

            this->StartPosition =
                System::Windows::Forms::FormStartPosition::CenterScreen;

            this->FormBorderStyle =
                System::Windows::Forms::FormBorderStyle::None;

            this->MaximizeBox = false;
            this->MinimizeBox = false;

            this->WindowState =
                System::Windows::Forms::FormWindowState::Maximized;

            this->BackColor = ThemeManager::Canvas();
            this->ForeColor = ThemeManager::Ink();

            this->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    10.0F,
                    System::Drawing::FontStyle::Regular
                );


            // =====================================================
            // CARD
            // =====================================================

            this->card = gcnew System::Windows::Forms::Panel();
            this->card->Size = System::Drawing::Size(400, 520);
            this->card->Location = System::Drawing::Point(50, 40);
            this->card->BackColor = ThemeManager::Surface();
            this->card->BorderStyle =
                System::Windows::Forms::BorderStyle::None;


            // =====================================================
            // CLOSE
            // =====================================================

            this->btnClose = gcnew System::Windows::Forms::Button();
            this->btnClose->Text = L"\u00D7";
            this->btnClose->Size = System::Drawing::Size(42, 42);
            this->btnClose->Location =
                System::Drawing::Point(this->ClientSize.Width - 58, 18);
            this->btnClose->Anchor =
                System::Windows::Forms::AnchorStyles::Top |
                System::Windows::Forms::AnchorStyles::Right;
            this->btnClose->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    16.0F,
                    System::Drawing::FontStyle::Regular
                );
            this->btnClose->ForeColor = ThemeManager::TextSecondary();
            this->btnClose->BackColor = System::Drawing::Color::Transparent;
            this->btnClose->FlatStyle =
                System::Windows::Forms::FlatStyle::Flat;
            this->btnClose->FlatAppearance->BorderSize = 0;
            this->btnClose->FlatAppearance->MouseOverBackColor =
                ThemeManager::Divider();
            this->btnClose->FlatAppearance->MouseDownBackColor =
                ThemeManager::Border();
            this->btnClose->Cursor =
                System::Windows::Forms::Cursors::Hand;
            this->btnClose->TabStop = false;


            // =====================================================
            // BRAND
            // =====================================================

            System::Windows::Forms::Label^ brandMark =
                gcnew System::Windows::Forms::Label();

            brandMark->Text = L"SC";
            brandMark->Size = System::Drawing::Size(40, 40);
            brandMark->Location = System::Drawing::Point(40, 36);
            brandMark->BackColor = ThemeManager::Ink();
            brandMark->ForeColor = System::Drawing::Color::White;
            brandMark->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    11.0F,
                    System::Drawing::FontStyle::Regular
                );
            brandMark->TextAlign =
                System::Drawing::ContentAlignment::MiddleCenter;

            System::Windows::Forms::Label^ brandName =
                gcnew System::Windows::Forms::Label();

            brandName->Text = L"SchoolCore";
            brandName->AutoSize = true;
            brandName->Location = System::Drawing::Point(90, 44);
            brandName->ForeColor = ThemeManager::Ink();
            brandName->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    12.0F,
                    System::Drawing::FontStyle::Regular
                );


            // =====================================================
            // TITLE AND SUBTITLE
            // =====================================================

            this->lblTitle = gcnew System::Windows::Forms::Label();
            this->lblTitle->Text = L"Sign in";
            this->lblTitle->AutoSize = true;
            this->lblTitle->Location = System::Drawing::Point(40, 96);
            this->lblTitle->ForeColor = ThemeManager::Ink();
            this->lblTitle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    20.0F,
                    System::Drawing::FontStyle::Regular
                );

            this->lblSubtitle = gcnew System::Windows::Forms::Label();
            this->lblSubtitle->Text =
                L"Enter your credentials to access the system.";
            this->lblSubtitle->AutoSize = true;
            this->lblSubtitle->Location = System::Drawing::Point(42, 140);
            this->lblSubtitle->ForeColor = ThemeManager::TextMuted();
            this->lblSubtitle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.5F,
                    System::Drawing::FontStyle::Regular
                );


            // =====================================================
            // USERNAME
            // =====================================================

            this->lblUsername = gcnew System::Windows::Forms::Label();
            this->lblUsername->Text = L"Username";
            this->lblUsername->AutoSize = true;
            this->lblUsername->Location = System::Drawing::Point(40, 186);
            this->lblUsername->ForeColor = ThemeManager::TextSecondary();
            this->lblUsername->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    9.0F,
                    System::Drawing::FontStyle::Regular
                );

            this->userField = gcnew System::Windows::Forms::Panel();
            this->userField->Size = System::Drawing::Size(320, 44);
            this->userField->Location = System::Drawing::Point(40, 210);
            this->userField->BackColor = ThemeManager::Border();
            this->userField->Padding =
                System::Windows::Forms::Padding(1);
            this->userField->TabIndex = 0;

            System::Windows::Forms::Panel^ userInner =
                gcnew System::Windows::Forms::Panel();
            userInner->Dock = System::Windows::Forms::DockStyle::Fill;
            userInner->BackColor = ThemeManager::Surface();
            userInner->Padding =
                System::Windows::Forms::Padding(12, 11, 12, 0);

            this->txtUsername = gcnew System::Windows::Forms::TextBox();
            this->txtUsername->Dock =
                System::Windows::Forms::DockStyle::Fill;
            this->txtUsername->BorderStyle =
                System::Windows::Forms::BorderStyle::None;
            this->txtUsername->BackColor = ThemeManager::Surface();
            this->txtUsername->ForeColor = ThemeManager::Ink();
            this->txtUsername->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    10.5F
                );
            this->txtUsername->TabIndex = 0;

            userInner->Controls->Add(this->txtUsername);
            this->userField->Controls->Add(userInner);


            // =====================================================
            // PASSWORD
            // =====================================================

            this->lblPassword = gcnew System::Windows::Forms::Label();
            this->lblPassword->Text = L"Password";
            this->lblPassword->AutoSize = true;
            this->lblPassword->Location = System::Drawing::Point(40, 268);
            this->lblPassword->ForeColor = ThemeManager::TextSecondary();
            this->lblPassword->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    9.0F,
                    System::Drawing::FontStyle::Regular
                );

            this->passField = gcnew System::Windows::Forms::Panel();
            this->passField->Size = System::Drawing::Size(320, 44);
            this->passField->Location = System::Drawing::Point(40, 292);
            this->passField->BackColor = ThemeManager::Border();
            this->passField->Padding =
                System::Windows::Forms::Padding(1);
            this->passField->TabIndex = 1;

            System::Windows::Forms::Panel^ passInner =
                gcnew System::Windows::Forms::Panel();
            passInner->Dock = System::Windows::Forms::DockStyle::Fill;
            passInner->BackColor = ThemeManager::Surface();
            passInner->Padding =
                System::Windows::Forms::Padding(12, 11, 12, 0);

            this->txtPassword = gcnew System::Windows::Forms::TextBox();
            this->txtPassword->Dock =
                System::Windows::Forms::DockStyle::Fill;
            this->txtPassword->BorderStyle =
                System::Windows::Forms::BorderStyle::None;
            this->txtPassword->BackColor = ThemeManager::Surface();
            this->txtPassword->ForeColor = ThemeManager::Ink();
            this->txtPassword->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    10.5F
                );
            this->txtPassword->UseSystemPasswordChar = true;
            this->txtPassword->TabIndex = 0;

            passInner->Controls->Add(this->txtPassword);
            this->passField->Controls->Add(passInner);


            // =====================================================
            // SHOW PASSWORD
            // =====================================================

            this->chkShowPassword =
                gcnew System::Windows::Forms::CheckBox();
            this->chkShowPassword->Text = L"Show password";
            this->chkShowPassword->AutoSize = true;
            this->chkShowPassword->Location =
                System::Drawing::Point(40, 350);
            this->chkShowPassword->ForeColor =
                ThemeManager::TextSecondary();
            this->chkShowPassword->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.0F
                );
            this->chkShowPassword->TabIndex = 2;


            // =====================================================
            // SIGN IN
            // =====================================================

            this->btnLogin = gcnew System::Windows::Forms::Button();
            this->btnLogin->Text = L"Sign in";
            this->btnLogin->Size = System::Drawing::Size(320, 46);
            this->btnLogin->Location = System::Drawing::Point(40, 384);
            this->btnLogin->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    10.5F,
                    System::Drawing::FontStyle::Regular
                );
            this->btnLogin->TabIndex = 3;
            ThemeManager::StylePrimaryButton(this->btnLogin);


            // =====================================================
            // MESSAGE
            // =====================================================

            this->lblMessage = gcnew System::Windows::Forms::Label();
            this->lblMessage->Text = L"";
            this->lblMessage->Size = System::Drawing::Size(320, 36);
            this->lblMessage->Location = System::Drawing::Point(40, 440);
            this->lblMessage->TextAlign =
                System::Drawing::ContentAlignment::MiddleLeft;
            this->lblMessage->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.0F
                );


            // =====================================================
            // FOOTER
            // =====================================================

            System::Windows::Forms::Label^ footer =
                gcnew System::Windows::Forms::Label();

            footer->Text =
                L"Secondary School Management System";
            footer->Dock = System::Windows::Forms::DockStyle::Bottom;
            footer->Height = 36;
            footer->ForeColor = ThemeManager::TextMuted();
            footer->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    8.5F
                );
            footer->TextAlign =
                System::Drawing::ContentAlignment::MiddleCenter;


            // =====================================================
            // ADD CONTROLS
            // =====================================================

            this->card->Controls->Add(footer);
            this->card->Controls->Add(this->lblMessage);
            this->card->Controls->Add(this->btnLogin);
            this->card->Controls->Add(this->chkShowPassword);
            this->card->Controls->Add(this->passField);
            this->card->Controls->Add(this->lblPassword);
            this->card->Controls->Add(this->userField);
            this->card->Controls->Add(this->lblUsername);
            this->card->Controls->Add(this->lblSubtitle);
            this->card->Controls->Add(this->lblTitle);
            this->card->Controls->Add(brandName);
            this->card->Controls->Add(brandMark);

            this->Controls->Add(this->card);
            this->Controls->Add(this->btnClose);


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

            this->txtUsername->Enter +=
                gcnew EventHandler(this, &LoginForm::Field_Enter);

            this->txtUsername->Leave +=
                gcnew EventHandler(this, &LoginForm::Field_Leave);

            this->txtPassword->Enter +=
                gcnew EventHandler(this, &LoginForm::Field_Enter);

            this->txtPassword->Leave +=
                gcnew EventHandler(this, &LoginForm::Field_Leave);

            this->card->Paint +=
                gcnew System::Windows::Forms::PaintEventHandler(
                    this,
                    &LoginForm::card_Paint
                );

            this->AcceptButton = this->btnLogin;
            this->CancelButton = this->btnClose;
            this->ActiveControl = this->txtUsername;

            this->Resize +=
                gcnew EventHandler(
                    this,
                    &LoginForm::LoginForm_Resize
                );

            this->ResumeLayout(false);

            CenterLoginCard();
        }

#pragma endregion

    public:

    };
}

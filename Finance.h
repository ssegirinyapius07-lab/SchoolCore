#pragma once

#include "AuthSession.h"
#include "ThemeManager.h"
#include "DbConnection.h"

#include <mariadb/conncpp.hpp>
#include <msclr/marshal_cppstd.h>

#include <memory>
#include <string>

namespace SchoolCore
{
    using namespace System;
    using namespace System::Drawing;
    using namespace System::Windows::Forms;

    private ref class FinanceTheme abstract sealed
    {
    public:
        static property Color Surface { Color get() { return Color::FromArgb(248, 250, 252); } }
        static property Color Hover { Color get() { return Color::FromArgb(241, 245, 249); } }
        static property Color Pressed { Color get() { return Color::FromArgb(226, 232, 240); } }
        static property Color TextStrong { Color get() { return Color::FromArgb(15, 23, 42); } }
        static property Color TextHeading { Color get() { return Color::FromArgb(30, 41, 59); } }
        static property Color TextMuted { Color get() { return Color::FromArgb(71, 85, 105); } }

        static initonly Drawing::Font^ Title = gcnew Drawing::Font(L"Segoe UI Semibold", 21.0F);
        static initonly Drawing::Font^ Dialog = gcnew Drawing::Font(L"Segoe UI Semibold", 18.0F);
        static initonly Drawing::Font^ Section = gcnew Drawing::Font(L"Segoe UI Semibold", 13.0F);
        static initonly Drawing::Font^ Metric = gcnew Drawing::Font(L"Segoe UI Semibold", 18.0F);
        static initonly Drawing::Font^ Action = gcnew Drawing::Font(L"Segoe UI Semibold", 10.0F);
        static initonly Drawing::Font^ Body = gcnew Drawing::Font(L"Segoe UI", 10.0F);
        static initonly Drawing::Font^ Small = gcnew Drawing::Font(L"Segoe UI", 9.0F);
    };

    private ref class FinanceOperation sealed
    {
    public:
        FinanceOperation(String^ title, String^ description)
            : Title(title), Description(description) {
        }

        initonly String^ Title;
        initonly String^ Description;
    };

    private ref class FinanceStudentItem sealed
    {
    public:
        int Id;
        String^ RegistrationNumber;
        String^ Name;

        FinanceStudentItem(
            int id,
            String^ registrationNumber,
            String^ name)
            : Id(id),
              RegistrationNumber(registrationNumber),
              Name(name)
        {
        }

        virtual String^ ToString() override
        {
            if (Id <= 0)
                return Name;

            return RegistrationNumber + L" - " + Name;
        }
    };

    private ref class FinanceChargeItem sealed
    {
    public:
        int Id;
        String^ FeeName;
        String^ AcademicYear;
        String^ Term;
        Decimal Balance;

        FinanceChargeItem(
            int id,
            String^ feeName,
            String^ academicYear,
            String^ term,
            Decimal balance)
            : Id(id),
              FeeName(feeName),
              AcademicYear(academicYear),
              Term(term),
              Balance(balance)
        {
        }

        virtual String^ ToString() override
        {
            return FeeName +
                L" | " +
                AcademicYear +
                L" | " +
                Term +
                L" | Balance UGX " +
                Balance.ToString(
                    L"N2",
                    Globalization::CultureInfo::InvariantCulture);
        }
    };

    public ref class Finance : public Form
    {
    public:
        Finance()
        {
            InitializeComponent();
                            ThemeManager::ApplyToForm(this);
                    }

    private:
        literal String^ PermView = L"fees.view";
        literal String^ PermManage = L"fees.manage";
        literal int MetricCount = 4;

        array<Label^>^ metricValues;

        Form^ paymentForm;
        ComboBox^ paymentStudentBox;
        ComboBox^ paymentChargeBox;
        ComboBox^ paymentMethodBox;
        ComboBox^ paymentProviderBox;
        DateTimePicker^ paymentDatePicker;
        TextBox^ paymentAmountBox;
        TextBox^ paymentReferenceBox;
        TextBox^ paymentPayerContactBox;
        TextBox^ paymentRemarksBox;
        Label^ paymentBalanceLabel;
        Label^ paymentStudentInfoLabel;
        Button^ paymentSaveButton;

        static Label^ CreateLabel(String^ text, Drawing::Font^ font, Color color)
        {
            Label^ label = gcnew Label();
            label->Text = text;
            label->Font = font;
            label->ForeColor = color;
            label->UseMnemonic = false;
            label->AutoEllipsis = true;
            return label;
        }

        static Panel^ CreateHeader(String^ title, String^ subtitle,
            Drawing::Font^ titleFont, int height, int titleHeight)
        {
            Panel^ header = gcnew Panel();
            header->Dock = DockStyle::Top;
            header->Height = height;
            header->BackColor = FinanceTheme::Surface;
            header->Padding = System::Windows::Forms::Padding(28, 18, 28, 12);

            Label^ heading = CreateLabel(title, titleFont, FinanceTheme::TextStrong);
            heading->Dock = DockStyle::Top;
            heading->Height = titleHeight;

            Label^ description = CreateLabel(subtitle, FinanceTheme::Body, FinanceTheme::TextMuted);
            description->Dock = DockStyle::Fill;
            description->AutoEllipsis = false;

            header->Controls->Add(description);
            header->Controls->Add(heading);
            return header;
        }

        Panel^ CreateMetricCard(String^ name, int index)
        {
            Panel^ card = gcnew Panel();
            card->Dock = DockStyle::Fill;
            card->Margin = System::Windows::Forms::Padding(0, 0, 12, 10);
            card->Padding = System::Windows::Forms::Padding(15, 12, 15, 12);
            card->BackColor = FinanceTheme::Surface;
            card->BorderStyle = BorderStyle::FixedSingle;

            Label^ caption = CreateLabel(name, FinanceTheme::Small, FinanceTheme::TextMuted);
            caption->Dock = DockStyle::Top;
            caption->Height = 28;

            Label^ value = CreateLabel(L"UGX 0.00", FinanceTheme::Metric, FinanceTheme::TextStrong);
            value->Dock = DockStyle::Fill;
            value->TextAlign = ContentAlignment::MiddleLeft;
            value->AccessibleName = name;

            metricValues[index] = value;

            card->Controls->Add(value);
            card->Controls->Add(caption);
            return card;
        }

        Button^ CreateActionButton(FinanceOperation^ operation, String^ name)
        {
            Button^ button = gcnew Button();
            button->Name = name;
            button->Text = operation->Title;
            button->Tag = operation;
            button->Dock = DockStyle::Fill;
            button->Margin = System::Windows::Forms::Padding(0, 0, 12, 12);
            button->Padding = System::Windows::Forms::Padding(18, 0, 10, 0);
            button->TextAlign = ContentAlignment::MiddleLeft;
            button->Font = FinanceTheme::Action;
            button->ForeColor = FinanceTheme::TextStrong;
            button->BackColor = Color::White;
            button->UseVisualStyleBackColor = false;
            button->FlatStyle = FlatStyle::Flat;
            button->FlatAppearance->BorderSize = 1;
            button->FlatAppearance->MouseOverBackColor = FinanceTheme::Hover;
            button->FlatAppearance->MouseDownBackColor = FinanceTheme::Pressed;
            button->Enabled = AuthSession::HasPermission(PermView);
            button->Click += gcnew EventHandler(this, &Finance::OnOperationClick);
            return button;
        }

        String^ GetSelectedPaymentMethodName()
        {
            if (paymentMethodBox == nullptr ||
                paymentMethodBox->SelectedIndex < 0)
            {
                return L"";
            }

            return paymentMethodBox->SelectedItem == nullptr
                ? L""
                : paymentMethodBox->SelectedItem->ToString();
        }

        String^ GetSelectedPaymentMethodCode()
        {
            switch (paymentMethodBox->SelectedIndex)
            {
            case 0:
                return L"MOBILE_MONEY";
            case 1:
                return L"BANK";
            case 2:
                return L"ONLINE_ELECTRONIC";
            default:
                return L"";
            }
        }

        void LoadPaymentStudents()
        {
            paymentStudentBox->Items->Clear();
            paymentStudentBox->Items->Add(
                gcnew FinanceStudentItem(
                    0,
                    L"",
                    L"Select student"
                )
            );

            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT DISTINCT "
                        "s.student_id, "
                        "s.registration_number, "
                        "CONCAT_WS(' ', s.first_name, s.middle_name, s.last_name) AS student_name "
                        "FROM students s "
                        "INNER JOIN fee_charges fc "
                        "ON fc.student_id = s.student_id "
                        "WHERE s.status = 'Active' "
                        "AND fc.status NOT IN ('Paid', 'Cancelled') "
                        "ORDER BY s.last_name, s.first_name"
                    )
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                while (result->next())
                {
                    paymentStudentBox->Items->Add(
                        gcnew FinanceStudentItem(
                            result->getInt("student_id"),
                            gcnew String(
                                result->getString(
                                    "registration_number").c_str()),
                            gcnew String(
                                result->getString(
                                    "student_name").c_str())
                        )
                    );
                }

                paymentStudentBox->SelectedIndex = 0;
                paymentStudentInfoLabel->Text =
                    L"Select a student to view outstanding charges.";
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Unable to Load Students",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }

        void LoadPaymentCharges(int studentId)
        {
            paymentChargeBox->Items->Clear();
            paymentChargeBox->Items->Add(
                gcnew FinanceChargeItem(
                    0,
                    L"Select fee charge",
                    L"",
                    L"",
                    Decimal(0)
                )
            );

            paymentBalanceLabel->Text = L"Outstanding: UGX 0.00";
            paymentAmountBox->Text = L"";

            if (studentId <= 0)
            {
                paymentChargeBox->SelectedIndex = 0;
                return;
            }

            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT "
                        "fc.fee_charge_id, "
                        "fs.fee_name, "
                        "ay.year_name, "
                        "t.term_name, "
                        "fc.amount - COALESCE(("
                        "    SELECT SUM(pa.amount) "
                        "    FROM payment_allocations pa "
                        "    WHERE pa.fee_charge_id = fc.fee_charge_id"
                        "), 0) AS balance "
                        "FROM fee_charges fc "
                        "INNER JOIN fee_structures fs "
                        "ON fs.fee_structure_id = fc.fee_structure_id "
                        "INNER JOIN academic_years ay "
                        "ON ay.academic_year_id = fs.academic_year_id "
                        "INNER JOIN terms t "
                        "ON t.term_id = fs.term_id "
                        "WHERE fc.student_id = ? "
                        "AND fc.status NOT IN ('Paid', 'Cancelled') "
                        "AND fc.amount - COALESCE(("
                        "    SELECT SUM(pa2.amount) "
                        "    FROM payment_allocations pa2 "
                        "    WHERE pa2.fee_charge_id = fc.fee_charge_id"
                        "), 0) > 0 "
                        "ORDER BY ay.start_date DESC, t.term_id, fs.fee_name"
                    )
                );

                stmt->setInt(1, studentId);

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                while (result->next())
                {
                    Decimal balance =
                        Decimal::Parse(
                            gcnew String(
                                result->getString("balance").c_str()
                            ),
                            Globalization::CultureInfo::InvariantCulture
                        );

                    paymentChargeBox->Items->Add(
                        gcnew FinanceChargeItem(
                            result->getInt("fee_charge_id"),
                            gcnew String(
                                result->getString("fee_name").c_str()),
                            gcnew String(
                                result->getString("year_name").c_str()),
                            gcnew String(
                                result->getString("term_name").c_str()),
                            balance
                        )
                    );
                }

                paymentChargeBox->SelectedIndex = 0;

                if (paymentChargeBox->Items->Count == 1)
                {
                    paymentStudentInfoLabel->Text =
                        L"No outstanding fee charges were found for this student.";
                }
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Unable to Load Fee Charges",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }

        void LoadPaymentProviders()
        {
            paymentProviderBox->Items->Clear();

            String^ methodCode =
                GetSelectedPaymentMethodCode();

            paymentProviderBox->Items->Add(L"Select provider");

            if (String::IsNullOrWhiteSpace(methodCode))
            {
                paymentProviderBox->SelectedIndex = 0;
                paymentProviderBox->DropDownStyle =
                    ComboBoxStyle::DropDownList;
                return;
            }

            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT pp.provider_name "
                        "FROM payment_providers pp "
                        "INNER JOIN payment_methods pm "
                        "ON pm.payment_method_id = pp.payment_method_id "
                        "WHERE pm.method_code = ? "
                        "AND pp.status = 'Active' "
                        "ORDER BY pp.provider_name"
                    )
                );

                stmt->setString(
                    1,
                    msclr::interop::marshal_as<std::string>(
                        methodCode
                    )
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                while (result->next())
                {
                    paymentProviderBox->Items->Add(
                        gcnew String(
                            result->getString(
                                "provider_name").c_str()
                        )
                    );
                }
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Unable to Load Payment Providers",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }

            paymentProviderBox->SelectedIndex = 0;

            paymentProviderBox->DropDownStyle =
                methodCode->Equals(L"MOBILE_MONEY")
                ? ComboBoxStyle::DropDownList
                : ComboBoxStyle::DropDown;
        }

        void UpdatePaymentChargeSelection()
        {
            FinanceChargeItem^ charge =
                dynamic_cast<FinanceChargeItem^>(
                    paymentChargeBox->SelectedItem
                );

            if (charge == nullptr || charge->Id <= 0)
            {
                paymentBalanceLabel->Text =
                    L"Outstanding: UGX 0.00";
                paymentAmountBox->Text = L"";
                return;
            }

            paymentBalanceLabel->Text =
                L"Outstanding: UGX " +
                charge->Balance.ToString(
                    L"N2",
                    Globalization::CultureInfo::InvariantCulture
                );

            paymentAmountBox->Text =
                charge->Balance.ToString(
                    L"N2",
                    Globalization::CultureInfo::InvariantCulture
                );
        }

        void PaymentStudentChanged(Object^ sender, EventArgs^ e)
        {
            FinanceStudentItem^ student =
                dynamic_cast<FinanceStudentItem^>(
                    paymentStudentBox->SelectedItem
                );

            if (student == nullptr)
                return;

            paymentStudentInfoLabel->Text =
                student->Id <= 0
                ? L"Select a student to view outstanding charges."
                : student->RegistrationNumber +
                  L"  |  " +
                  student->Name;

            LoadPaymentCharges(student->Id);
        }

        void PaymentChargeChanged(Object^ sender, EventArgs^ e)
        {
            UpdatePaymentChargeSelection();
        }

        void PaymentMethodChanged(Object^ sender, EventArgs^ e)
        {
            LoadPaymentProviders();
        }

        int GetPaymentProviderId(
            sql::Connection* con,
            int paymentMethodId,
            String^ providerName)
        {
            if (con == nullptr ||
                paymentMethodId <= 0 ||
                String::IsNullOrWhiteSpace(providerName))
            {
                return 0;
            }

            std::unique_ptr<sql::PreparedStatement> findStmt(
                con->prepareStatement(
                    "SELECT payment_provider_id "
                    "FROM payment_providers "
                    "WHERE payment_method_id = ? "
                    "AND LOWER(provider_name) = LOWER(?) "
                    "AND status = 'Active' "
                    "LIMIT 1"
                )
            );

            findStmt->setInt(1, paymentMethodId);
            findStmt->setString(
                2,
                msclr::interop::marshal_as<std::string>(
                    providerName->Trim()
                )
            );

            std::unique_ptr<sql::ResultSet> found(
                findStmt->executeQuery()
            );

            if (found->next())
                return found->getInt("payment_provider_id");

            std::unique_ptr<sql::PreparedStatement> insertStmt(
                con->prepareStatement(
                    "INSERT INTO payment_providers "
                    "(payment_method_id, provider_code, provider_name, status) "
                    "VALUES (?, ?, ?, 'Active')"
                )
            );

            String^ code =
                GetPaymentMethodCode() +
                L"_" +
                providerName->Trim()->ToUpperInvariant();

            insertStmt->setInt(1, paymentMethodId);
            insertStmt->setString(
                2,
                msclr::interop::marshal_as<std::string>(
                    code
                )
            );
            insertStmt->setString(
                3,
                msclr::interop::marshal_as<std::string>(
                    providerName->Trim()
                )
            );

            insertStmt->execute();

            std::unique_ptr<sql::Statement> idStmt(
                con->createStatement()
            );

            std::unique_ptr<sql::ResultSet> idResult(
                idStmt->executeQuery(
                    "SELECT LAST_INSERT_ID() AS provider_id"
                )
            );

            return idResult->next()
                ? idResult->getInt("provider_id")
                : 0;
        }

        void SavePayment(Object^ sender, EventArgs^ e)
        {
            if (!AuthSession::HasPermission(PermManage))
            {
                MessageBox::Show(
                    paymentForm,
                    L"You do not have permission to record payments.",
                    L"Access denied",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                return;
            }

            FinanceStudentItem^ student =
                dynamic_cast<FinanceStudentItem^>(
                    paymentStudentBox->SelectedItem
                );

            FinanceChargeItem^ charge =
                dynamic_cast<FinanceChargeItem^>(
                    paymentChargeBox->SelectedItem
                );

            String^ method =
                GetSelectedPaymentMethodName();

            String^ provider =
                paymentProviderBox->Text->Trim();

            String^ reference =
                paymentReferenceBox->Text->Trim();

            if (student == nullptr || student->Id <= 0)
            {
                MessageBox::Show(
                    paymentForm,
                    L"Select a student first.",
                    L"Record Payment",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                return;
            }

            if (charge == nullptr || charge->Id <= 0)
            {
                MessageBox::Show(
                    paymentForm,
                    L"Select an outstanding fee charge.",
                    L"Record Payment",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                return;
            }

            if (String::IsNullOrWhiteSpace(method))
            {
                MessageBox::Show(
                    paymentForm,
                    L"Select a payment method.",
                    L"Record Payment",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                return;
            }

            if (String::IsNullOrWhiteSpace(provider) ||
                provider->Equals(L"Select provider"))
            {
                MessageBox::Show(
                    paymentForm,
                    L"Enter or select the payment provider.",
                    L"Record Payment",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                return;
            }

            if (String::IsNullOrWhiteSpace(reference))
            {
                MessageBox::Show(
                    paymentForm,
                    L"Enter the transaction reference.",
                    L"Record Payment",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                return;
            }

            Decimal amount;

            try
            {
                amount =
                    Decimal::Parse(
                        paymentAmountBox->Text->Trim(),
                        Globalization::NumberStyles::Number,
                        Globalization::CultureInfo::InvariantCulture
                    );
            }
            catch (Exception^)
            {
                MessageBox::Show(
                    paymentForm,
                    L"Enter a valid payment amount.",
                    L"Record Payment",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                return;
            }

            if (amount <= Decimal(0))
            {
                MessageBox::Show(
                    paymentForm,
                    L"Payment amount must be greater than zero.",
                    L"Record Payment",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                return;
            }

            if (amount > charge->Balance)
            {
                MessageBox::Show(
                    paymentForm,
                    L"The payment cannot be greater than the outstanding balance.",
                    L"Record Payment",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                return;
            }

            try
            {
                auto conOwner = DbConnection::GetConnection();
                sql::Connection* con = conOwner.get();

                int methodId = 0;

                {
                    std::unique_ptr<sql::PreparedStatement> methodStmt(
                        con->prepareStatement(
                            "SELECT payment_method_id "
                            "FROM payment_methods "
                            "WHERE method_code = ? "
                            "AND status = 'Active' "
                            "LIMIT 1"
                        )
                    );

                    methodStmt->setString(
                        1,
                        msclr::interop::marshal_as<std::string>(
                            GetSelectedPaymentMethodCode()
                        )
                    );

                    std::unique_ptr<sql::ResultSet> methodResult(
                        methodStmt->executeQuery()
                    );

                    if (methodResult->next())
                        methodId =
                            methodResult->getInt(
                                "payment_method_id"
                            );
                }

                if (methodId <= 0)
                {
                    throw gcnew Exception(
                        L"The selected payment method is not configured."
                    );
                }

                con->setAutoCommit(false);

                int providerId =
                    GetPaymentProviderId(
                        con,
                        methodId,
                        provider
                    );

                if (providerId <= 0)
                {
                    throw gcnew Exception(
                        L"The payment provider could not be saved."
                    );
                }

                std::unique_ptr<sql::PreparedStatement> insertPayment(
                    con->prepareStatement(
                        "INSERT INTO payments "
                        "(student_id, receipt_number, payment_date, amount, "
                        "payment_method, payment_method_id, payment_provider_id, "
                        "payer_contact, transaction_reference, payment_status, "
                        "received_by, verified_by, verified_at, remarks) "
                        "VALUES (?, 'PENDING', ?, ?, ?, ?, ?, ?, ?, "
                        "'Confirmed', ?, ?, NOW(), ?)"
                    )
                );

                insertPayment->setInt(1, student->Id);
                insertPayment->setString(
                    2,
                    msclr::interop::marshal_as<std::string>(
                        paymentDatePicker->Value.ToString(L"yyyy-MM-dd")
                    )
                );
                insertPayment->setDouble(
                    3,
                    Convert::ToDouble(amount)
                );
                insertPayment->setString(
                    4,
                    msclr::interop::marshal_as<std::string>(
                        method
                    )
                );
                insertPayment->setInt(5, methodId);
                insertPayment->setInt(6, providerId);
                insertPayment->setString(
                    7,
                    msclr::interop::marshal_as<std::string>(
                        paymentPayerContactBox->Text->Trim()
                    )
                );
                insertPayment->setString(
                    8,
                    msclr::interop::marshal_as<std::string>(
                        reference
                    )
                );
                insertPayment->setInt(
                    9,
                    AuthSession::UserId
                );
                insertPayment->setInt(
                    10,
                    AuthSession::UserId
                );
                insertPayment->setString(
                    11,
                    msclr::interop::marshal_as<std::string>(
                        paymentRemarksBox->Text->Trim()
                    )
                );

                insertPayment->execute();

                std::unique_ptr<sql::Statement> idStmt(
                    con->createStatement()
                );

                std::unique_ptr<sql::ResultSet> idResult(
                    idStmt->executeQuery(
                        "SELECT LAST_INSERT_ID() AS payment_id"
                    )
                );

                if (!idResult->next())
                    throw gcnew Exception(
                        L"Unable to generate the payment number."
                    );

                int paymentId =
                    idResult->getInt("payment_id");

                String^ receipt =
                    L"SC-RCP-" +
                    DateTime::Now.ToString(L"yyyy") +
                    L"-" +
                    paymentId.ToString(L"D6");

                std::unique_ptr<sql::PreparedStatement> updateReceipt(
                    con->prepareStatement(
                        "UPDATE payments "
                        "SET receipt_number = ? "
                        "WHERE payment_id = ?"
                    )
                );

                updateReceipt->setString(
                    1,
                    msclr::interop::marshal_as<std::string>(
                        receipt
                    )
                );
                updateReceipt->setInt(2, paymentId);
                updateReceipt->execute();

                std::unique_ptr<sql::PreparedStatement> insertAllocation(
                    con->prepareStatement(
                        "INSERT INTO payment_allocations "
                        "(payment_id, fee_charge_id, amount) "
                        "VALUES (?, ?, ?)"
                    )
                );

                insertAllocation->setInt(1, paymentId);
                insertAllocation->setInt(2, charge->Id);
                insertAllocation->setDouble(
                    3,
                    Convert::ToDouble(amount)
                );
                insertAllocation->execute();

                Decimal remaining =
                    charge->Balance - amount;

                std::unique_ptr<sql::PreparedStatement> updateCharge(
                    con->prepareStatement(
                        "UPDATE fee_charges "
                        "SET status = ? "
                        "WHERE fee_charge_id = ?"
                    )
                );

                updateCharge->setString(
                    1,
                    remaining <= Decimal(0)
                    ? "Paid"
                    : "Partially Paid"
                );
                updateCharge->setInt(2, charge->Id);
                updateCharge->execute();

                con->commit();
                con->setAutoCommit(true);

                MessageBox::Show(
                    paymentForm,
                    L"Payment recorded successfully.\n\n"
                    L"Receipt: " + receipt +
                    L"\nStudent: " + student->Name +
                    L"\nAmount: UGX " +
                    amount.ToString(
                        L"N2",
                        Globalization::CultureInfo::InvariantCulture) +
                    L"\nMethod: " + method +
                    L"\nProvider: " + provider +
                    L"\nReference: " + reference,
                    L"Payment Recorded",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information
                );

                paymentForm->DialogResult =
                    System::Windows::Forms::DialogResult::OK;

                LoadPaymentStudents();
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    paymentForm,
                    gcnew String(ex.what()),
                    L"Unable to Record Payment",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
            catch (Exception^ ex)
            {
                MessageBox::Show(
                    paymentForm,
                    ex->Message,
                    L"Unable to Record Payment",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }

        void OpenRecordPaymentDialog()
        {
            if (!AuthSession::HasPermission(PermManage))
            {
                MessageBox::Show(
                    this,
                    L"You do not have permission to record payments.",
                    L"SchoolCore - Access denied",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                return;
            }

            paymentForm = gcnew Form();
            paymentForm->Text = L"SchoolCore - Record Payment";
            paymentForm->StartPosition = FormStartPosition::CenterParent;
            paymentForm->FormBorderStyle =
                System::Windows::Forms::FormBorderStyle::FixedDialog;
            paymentForm->MaximizeBox = false;
            paymentForm->MinimizeBox = false;
            paymentForm->ShowInTaskbar = false;
            paymentForm->ClientSize = Drawing::Size(820, 720);
            paymentForm->MinimumSize = Drawing::Size(780, 680);
            paymentForm->BackColor = ThemeManager::Canvas();

            TableLayoutPanel^ root =
                gcnew TableLayoutPanel();

            root->Dock = DockStyle::Fill;
            root->Padding =
                System::Windows::Forms::Padding(28);
            root->ColumnCount = 2;
            root->RowCount = 12;
            root->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Absolute, 170.0F));
            root->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Percent, 100.0F));

            root->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 48.0F));
            root->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 52.0F));
            root->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 28.0F));
            root->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 52.0F));
            root->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 54.0F));
            root->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 52.0F));
            root->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 52.0F));
            root->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 52.0F));
            root->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 52.0F));
            root->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 52.0F));
            root->RowStyles->Add(
                gcnew RowStyle(SizeType::Percent, 100.0F));
            root->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 58.0F));

            Label^ title =
                CreateLabel(
                    L"Record Payment",
                    FinanceTheme::Dialog,
                    FinanceTheme::TextStrong);
            title->Dock = DockStyle::Fill;

            Label^ studentInfo =
                CreateLabel(
                    L"Select a student to view outstanding charges.",
                    FinanceTheme::Small,
                    FinanceTheme::TextMuted);
            studentInfo->Dock = DockStyle::Fill;
            studentInfo->AutoEllipsis = true;
            paymentStudentInfoLabel = studentInfo;

            paymentStudentBox = gcnew ComboBox();
            paymentStudentBox->Dock = DockStyle::Fill;
            paymentStudentBox->DropDownStyle =
                ComboBoxStyle::DropDownList;

            paymentChargeBox = gcnew ComboBox();
            paymentChargeBox->Dock = DockStyle::Fill;
            paymentChargeBox->DropDownStyle =
                ComboBoxStyle::DropDownList;

            paymentAmountBox = gcnew TextBox();
            paymentAmountBox->Dock = DockStyle::Fill;
            paymentAmountBox->TextAlign =
                HorizontalAlignment::Right;

            paymentDatePicker = gcnew DateTimePicker();
            paymentDatePicker->Dock = DockStyle::Fill;
            paymentDatePicker->Format =
                DateTimePickerFormat::Short;
            paymentDatePicker->Value = DateTime::Today;

            paymentMethodBox = gcnew ComboBox();
            paymentMethodBox->Dock = DockStyle::Fill;
            paymentMethodBox->DropDownStyle =
                ComboBoxStyle::DropDownList;
            paymentMethodBox->Items->Add(L"Mobile Money");
            paymentMethodBox->Items->Add(L"Bank");
            paymentMethodBox->Items->Add(
                L"Online/Electronic Payment");
            paymentMethodBox->SelectedIndex = 0;

            paymentProviderBox = gcnew ComboBox();
            paymentProviderBox->Dock = DockStyle::Fill;

            paymentReferenceBox = gcnew TextBox();
            paymentReferenceBox->Dock = DockStyle::Fill;
            paymentReferenceBox->MaxLength = 100;

            paymentPayerContactBox = gcnew TextBox();
            paymentPayerContactBox->Dock = DockStyle::Fill;
            paymentPayerContactBox->MaxLength = 50;

            paymentRemarksBox = gcnew TextBox();
            paymentRemarksBox->Dock = DockStyle::Fill;
            paymentRemarksBox->Multiline = true;
            paymentRemarksBox->ScrollBars =
                ScrollBars::Vertical;
            paymentRemarksBox->MaxLength = 255;

            paymentBalanceLabel =
                gcnew Label();
            paymentBalanceLabel->Text =
                L"Outstanding: UGX 0.00";
            paymentBalanceLabel->Dock =
                DockStyle::Fill;
            paymentBalanceLabel->TextAlign =
                ContentAlignment::MiddleRight;
            paymentBalanceLabel->ForeColor =
                ThemeManager::TextSecondary();

            Panel^ amountPanel = gcnew Panel();
            amountPanel->Dock = DockStyle::Fill;
            amountPanel->Padding =
                System::Windows::Forms::Padding(0, 0, 0, 0);
            amountPanel->Controls->Add(paymentAmountBox);
            amountPanel->Controls->Add(paymentBalanceLabel);

            paymentBalanceLabel->Dock =
                DockStyle::Bottom;
            paymentBalanceLabel->Height = 22;
            paymentAmountBox->Dock =
                DockStyle::Top;

            Button^ cancel = gcnew Button();
            cancel->Text = L"Cancel";
            cancel->Width = 110;
            cancel->DialogResult =
                System::Windows::Forms::DialogResult::Cancel;

            paymentSaveButton = gcnew Button();
            paymentSaveButton->Text = L"Record Payment";
            paymentSaveButton->Width = 150;

            FlowLayoutPanel^ footer =
                gcnew FlowLayoutPanel();
            footer->Dock = DockStyle::Fill;
            footer->FlowDirection =
                FlowDirection::RightToLeft;
            footer->WrapContents = false;
            footer->Controls->Add(cancel);
            footer->Controls->Add(paymentSaveButton);

            root->Controls->Add(title, 0, 0);
            root->SetColumnSpan(title, 2);

            root->Controls->Add(
                gcnew Label()
                {
                },
                0, 1);

            root->Controls->Add(paymentStudentBox, 1, 1);
            root->Controls->Add(
                CreateLabel(
                    L"Student",
                    FinanceTheme::Body,
                    FinanceTheme::TextStrong),
                0, 1);

            root->Controls->Add(paymentStudentInfoLabel, 1, 2);
            root->SetColumnSpan(paymentStudentInfoLabel, 2);

            root->Controls->Add(
                CreateLabel(
                    L"Fee Charge",
                    FinanceTheme::Body,
                    FinanceTheme::TextStrong),
                0, 3);
            root->Controls->Add(paymentChargeBox, 1, 3);

            root->Controls->Add(
                CreateLabel(
                    L"Amount (UGX)",
                    FinanceTheme::Body,
                    FinanceTheme::TextStrong),
                0, 4);
            root->Controls->Add(amountPanel, 1, 4);

            root->Controls->Add(
                CreateLabel(
                    L"Payment Date",
                    FinanceTheme::Body,
                    FinanceTheme::TextStrong),
                0, 5);
            root->Controls->Add(paymentDatePicker, 1, 5);

            root->Controls->Add(
                CreateLabel(
                    L"Payment Method",
                    FinanceTheme::Body,
                    FinanceTheme::TextStrong),
                0, 6);
            root->Controls->Add(paymentMethodBox, 1, 6);

            root->Controls->Add(
                CreateLabel(
                    L"Provider",
                    FinanceTheme::Body,
                    FinanceTheme::TextStrong),
                0, 7);
            root->Controls->Add(paymentProviderBox, 1, 7);

            root->Controls->Add(
                CreateLabel(
                    L"Reference",
                    FinanceTheme::Body,
                    FinanceTheme::TextStrong),
                0, 8);
            root->Controls->Add(
                paymentReferenceBox,
                1, 8);

            root->Controls->Add(
                CreateLabel(
                    L"Payer Contact",
                    FinanceTheme::Body,
                    FinanceTheme::TextStrong),
                0, 9);
            root->Controls->Add(
                paymentPayerContactBox,
                1, 9);

            root->Controls->Add(
                CreateLabel(
                    L"Remarks",
                    FinanceTheme::Body,
                    FinanceTheme::TextStrong),
                0, 10);
            root->Controls->Add(
                paymentRemarksBox,
                1, 10);

            root->Controls->Add(
                footer,
                0, 11);
            root->SetColumnSpan(footer, 2);

            paymentStudentBox->SelectedIndexChanged +=
                gcnew EventHandler(
                    this,
                    &Finance::PaymentStudentChanged
                );
            paymentChargeBox->SelectedIndexChanged +=
                gcnew EventHandler(
                    this,
                    &Finance::PaymentChargeChanged
                );
            paymentMethodBox->SelectedIndexChanged +=
                gcnew EventHandler(
                    this,
                    &Finance::PaymentMethodChanged
                );
            paymentSaveButton->Click +=
                gcnew EventHandler(
                    this,
                    &Finance::SavePayment
                );

            paymentForm->Controls->Add(root);

            ThemeManager::ApplyToForm(paymentForm);

            LoadPaymentProviders();
            LoadPaymentStudents();

            paymentForm->AcceptButton = paymentSaveButton;
            paymentForm->CancelButton = cancel;

            paymentForm->ShowDialog(this);

            delete paymentForm;
            paymentForm = nullptr;
        }

        void SetMetric(int index, Decimal amount)
        {
            if (index < 0 || index >= MetricCount)
                return;

            metricValues[index]->Text =
                L"UGX " + amount.ToString(
                    L"N2",
                    Globalization::CultureInfo::InvariantCulture);
        }

        void ShowRestrictedMetrics()
        {
            for (int i = 0; i < MetricCount; ++i)
                metricValues[i]->Text = L"Restricted";
        }

        void OnOperationClick(Object^ sender, EventArgs^ e)
        {
            Button^ button = dynamic_cast<Button^>(sender);
            if (button == nullptr)
                return;

            FinanceOperation^ operation =
                dynamic_cast<FinanceOperation^>(button->Tag);

            if (operation == nullptr)
                return;

            OpenFinanceOperation(operation);
        }

        void OpenFinanceOperation(FinanceOperation^ operation)
        {
            if (!AuthSession::HasPermission(PermView))
            {
                MessageBox::Show(
                    this,
                    L"You do not have permission to open this section.",
                    L"SchoolCore - Access denied",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning);
                return;
            }

            if (operation->Title->Equals(L"Record Payment"))
            {
                OpenRecordPaymentDialog();
                return;
            }

            Form^ view = gcnew Form();

            try
            {
                view->Text = L"SchoolCore - " + operation->Title;
                view->StartPosition = FormStartPosition::CenterParent;
                view->ClientSize = Drawing::Size(900, 560);
                view->MinimumSize = Drawing::Size(800, 500);
                view->BackColor = Color::White;
                view->ShowInTaskbar = false;
                view->MinimizeBox = false;
                view->MaximizeBox = false;

                Panel^ footer = gcnew Panel();
                footer->Dock = DockStyle::Bottom;
                footer->Height = 56;
                footer->Padding =
                    System::Windows::Forms::Padding(28, 10, 28, 10);

                Button^ close = gcnew Button();
                close->Text = L"Close";
                close->Dock = DockStyle::Right;
                close->Width = 110;
                close->DialogResult =
                    System::Windows::Forms::DialogResult::Cancel;

                footer->Controls->Add(close);
                view->CancelButton = close;

                view->Controls->Add(footer);
                view->Controls->Add(
                    CreateHeader(
                        operation->Title,
                        operation->Description,
                        FinanceTheme::Dialog,
                        96,
                        36));

                ThemeManager::ApplyToForm(view);
                view->ShowDialog(this);
            }
            finally
            {
                delete view;
            }
        }

        void InitializeComponent()
        {
            this->SuspendLayout();

            this->Text = L"SchoolCore - Fees & Finance";
            this->StartPosition = FormStartPosition::CenterScreen;
            this->AutoScaleDimensions = SizeF(96.0F, 96.0F);
            this->AutoScaleMode =
                System::Windows::Forms::AutoScaleMode::Dpi;
            this->ClientSize = Drawing::Size(1000, 650);
            this->MinimumSize = Drawing::Size(680, 560);
            this->BackColor = Color::White;
            this->DoubleBuffered = true;

            bool canView = AuthSession::HasPermission(PermView);
            metricValues = gcnew array<Label^>(MetricCount);

            TableLayoutPanel^ content = gcnew TableLayoutPanel();
            content->Dock = DockStyle::Fill;
            content->Padding = System::Windows::Forms::Padding(28);
            content->ColumnCount = 1;
            content->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Percent, 100.0F));
            content->RowCount = 5;
            content->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 42.0F));
            content->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 135.0F));
            content->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 45.0F));
            content->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 190.0F));
            content->RowStyles->Add(
                gcnew RowStyle(SizeType::Percent, 100.0F));

            Label^ overview =
                CreateLabel(
                    L"Financial Overview",
                    FinanceTheme::Section,
                    FinanceTheme::TextHeading);
            overview->Dock = DockStyle::Fill;

            TableLayoutPanel^ metrics = gcnew TableLayoutPanel();
            metrics->Dock = DockStyle::Fill;
            metrics->Margin = System::Windows::Forms::Padding(0);
            metrics->ColumnCount = 2;
            metrics->RowCount = 2;

            metrics->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Percent, 50.0F));
            metrics->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Percent, 50.0F));
            metrics->RowStyles->Add(
                gcnew RowStyle(SizeType::Percent, 50.0F));
            metrics->RowStyles->Add(
                gcnew RowStyle(SizeType::Percent, 50.0F));

            array<String^>^ metricNames = gcnew array<String^>
            {
                L"Total Revenue",
                L"Outstanding Fees",
                L"Collected Today",
                L"Pending Approvals"
            };

            for (int i = 0; i < MetricCount; ++i)
                metrics->Controls->Add(
                    CreateMetricCard(metricNames[i], i), i % 2, i / 2);

            if (!canView)
                ShowRestrictedMetrics();

            Label^ operations =
                CreateLabel(
                    L"Finance Operations",
                    FinanceTheme::Section,
                    FinanceTheme::TextHeading);
            operations->Dock = DockStyle::Fill;
            operations->Padding =
                System::Windows::Forms::Padding(0, 8, 0, 0);

            TableLayoutPanel^ actions = gcnew TableLayoutPanel();
            actions->Dock = DockStyle::Fill;
            actions->Margin = System::Windows::Forms::Padding(0);
            actions->ColumnCount = 2;
            actions->RowCount = 2;
            actions->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Percent, 50.0F));
            actions->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Percent, 50.0F));
            actions->RowStyles->Add(
                gcnew RowStyle(SizeType::Percent, 50.0F));
            actions->RowStyles->Add(
                gcnew RowStyle(SizeType::Percent, 50.0F));

            array<FinanceOperation^>^ definitions =
                gcnew array<FinanceOperation^>
            {
                gcnew FinanceOperation(
                    L"Fee Structures",
                    L"Define and manage school fee structures by academic year, term, class and stream."),
                gcnew FinanceOperation(
                    L"Student Charges",
                    L"Review student fee charges, due dates, payment status and outstanding balances."),
                gcnew FinanceOperation(
                    L"Record Payment",
                    L"Record Mobile Money, bank, or online/electronic payments with the provider and transaction reference."),
                gcnew FinanceOperation(
                    L"Payment History",
                    L"Review recorded payments, receipts, payment methods and transaction references.")
            };

            array<String^>^ buttonNames = gcnew array<String^>
            {
                L"btnFeeStructures",
                L"btnStudentCharges",
                L"btnRecordPayment",
                L"btnPaymentHistory"
            };

            for (int i = 0; i < definitions->Length; ++i)
            {
                Button^ button =
                    CreateActionButton(definitions[i], buttonNames[i]);

                button->TabIndex = i;
                actions->Controls->Add(
                    button,
                    i % 2,
                    i / 2);
            }

            String^ noteText = L"";

            if (!canView)
            {
                noteText =
                    L"Your account does not have access to financial records. "
                    L"Contact an administrator if you need it.";
            }

            Label^ note =
                CreateLabel(
                    noteText,
                    FinanceTheme::Small,
                    FinanceTheme::TextMuted);
            note->Dock = DockStyle::Fill;
            note->AutoEllipsis = false;
            note->Padding =
                System::Windows::Forms::Padding(0, 18, 0, 0);

            content->Controls->Add(overview, 0, 0);
            content->Controls->Add(metrics, 0, 1);
            content->Controls->Add(operations, 0, 2);
            content->Controls->Add(actions, 0, 3);
            content->Controls->Add(note, 0, 4);

            this->Controls->Add(content);
            this->Controls->Add(
                CreateHeader(
                    L"Fees & Finance",
                    L"Manage fee structures, student charges, payments and financial records.",
                    FinanceTheme::Title,
                    105,
                    42));

            this->ResumeLayout(false);
        }
    };
}

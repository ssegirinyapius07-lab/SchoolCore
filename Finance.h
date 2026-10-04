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

    private ref class FinanceLookupItem sealed
    {
    public:
        int Id;
        String^ Name;

        FinanceLookupItem(int id, String^ name)
            : Id(id), Name(name)
        {
        }

        virtual String^ ToString() override
        {
            return Name;
        }
    };

    private ref class FinanceFeeStructureItem sealed
    {
    public:
        int Id;
        String^ FeeName;
        String^ AcademicYear;
        String^ Term;
        String^ ClassName;
        String^ StreamName;
        Decimal Amount;
        String^ DueDate;

        FinanceFeeStructureItem(
            int id,
            String^ feeName,
            String^ academicYear,
            String^ term,
            String^ className,
            String^ streamName,
            Decimal amount,
            String^ dueDate)
            : Id(id),
              FeeName(feeName),
              AcademicYear(academicYear),
              Term(term),
              ClassName(className),
              StreamName(streamName),
              Amount(amount),
              DueDate(dueDate)
        {
        }

        virtual String^ ToString() override
        {
            return FeeName +
                L" | " + AcademicYear +
                L" | " + Term +
                L" | " + ClassName +
                L" | " + StreamName +
                L" | UGX " +
                Amount.ToString(
                    L"N2",
                    System::Globalization::CultureInfo::InvariantCulture);
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
                    System::Globalization::CultureInfo::InvariantCulture);
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

        Form^ feeStructuresForm;
        DataGridView^ feeStructuresGrid;
        Label^ feeStructuresSummaryLabel;

        Form^ feeStructureEditorForm;
        ComboBox^ feeStructureYearBox;
        ComboBox^ feeStructureTermBox;
        ComboBox^ feeStructureClassBox;
        ComboBox^ feeStructureStreamBox;
        TextBox^ feeStructureNameBox;
        TextBox^ feeStructureAmountBox;
        DateTimePicker^ feeStructureDueDatePicker;
        TextBox^ feeStructureDescriptionBox;

        Form^ studentChargesForm;
        DataGridView^ studentChargesGrid;
        ComboBox^ studentChargesStructureBox;
        Label^ studentChargesSummaryLabel;

        Form^ paymentHistoryForm;
        DataGridView^ paymentHistoryGrid;
        TextBox^ paymentHistorySearchBox;
        ComboBox^ paymentHistoryMethodBox;
        ComboBox^ paymentHistoryStatusBox;
        DateTimePicker^ paymentHistoryFromPicker;
        DateTimePicker^ paymentHistoryToPicker;
        Label^ paymentHistorySummaryLabel;
        Button^ paymentHistoryViewButton;

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

        static Label^ CreateFieldLabel(String^ text)
        {
            Label^ label =
                CreateLabel(
                    text,
                    FinanceTheme::Body,
                    FinanceTheme::TextStrong);

            label->Dock = DockStyle::Fill;
            label->AutoSize = false;
            label->AutoEllipsis = false;
            label->TextAlign = ContentAlignment::MiddleLeft;
            label->Margin =
                System::Windows::Forms::Padding(0, 0, 10, 0);

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
                        "SELECT "
                        "s.student_id, "
                        "s.registration_number, "
                        "CONCAT_WS(' ', s.first_name, s.middle_name, s.last_name) AS student_name "
                        "FROM students s "
                        "WHERE s.status = 'Active' "
                        "ORDER BY s.last_name, s.first_name, s.registration_number"
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
                            System::Globalization::CultureInfo::InvariantCulture
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
                    System::Globalization::CultureInfo::InvariantCulture
                );

            paymentAmountBox->Text =
                charge->Balance.ToString(
                    L"N2",
                    System::Globalization::CultureInfo::InvariantCulture
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
                GetSelectedPaymentMethodCode() +
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
                        System::Globalization::CultureInfo::InvariantCulture
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

            std::unique_ptr<sql::Connection> conOwner;
            sql::Connection* con = nullptr;
            bool transactionStarted = false;

            try
            {
                conOwner = DbConnection::GetConnection();
                con = conOwner.get();

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
                transactionStarted = true;

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

                std::unique_ptr<sql::PreparedStatement> duplicateReference(
                    con->prepareStatement(
                        "SELECT payment_id "
                        "FROM payments "
                        "WHERE payment_provider_id = ? "
                        "AND transaction_reference = ? "
                        "AND payment_status <> 'Cancelled' "
                        "LIMIT 1"
                    )
                );

                duplicateReference->setInt(
                    1,
                    providerId
                );
                duplicateReference->setString(
                    2,
                    msclr::interop::marshal_as<std::string>(
                        reference
                    )
                );

                std::unique_ptr<sql::ResultSet> duplicateResult(
                    duplicateReference->executeQuery()
                );

                if (duplicateResult->next())
                {
                    throw gcnew Exception(
                        L"This transaction reference has already been recorded for the selected provider."
                    );
                }

                Decimal liveBalance = Decimal(0);

                {
                    std::unique_ptr<sql::PreparedStatement> lockCharge(
                        con->prepareStatement(
                            "SELECT amount "
                            "FROM fee_charges "
                            "WHERE fee_charge_id = ? "
                            "AND student_id = ? "
                            "FOR UPDATE"
                        )
                    );

                    lockCharge->setInt(1, charge->Id);
                    lockCharge->setInt(2, student->Id);

                    std::unique_ptr<sql::ResultSet> chargeResult(
                        lockCharge->executeQuery()
                    );

                    if (!chargeResult->next())
                    {
                        throw gcnew Exception(
                            L"The selected fee charge is no longer available."
                        );
                    }

                    Decimal chargeAmount =
                        Decimal::Parse(
                            gcnew String(
                                chargeResult->getString(
                                    "amount").c_str()
                            ),
                            System::Globalization::CultureInfo::InvariantCulture
                        );

                    std::unique_ptr<sql::PreparedStatement> paidStmt(
                        con->prepareStatement(
                            "SELECT COALESCE(SUM(amount), 0) AS paid "
                            "FROM payment_allocations "
                            "WHERE fee_charge_id = ?"
                        )
                    );

                    paidStmt->setInt(1, charge->Id);

                    std::unique_ptr<sql::ResultSet> paidResult(
                        paidStmt->executeQuery()
                    );

                    Decimal paidAmount = Decimal(0);

                    if (paidResult->next())
                    {
                        paidAmount =
                            Decimal::Parse(
                                gcnew String(
                                    paidResult->getString(
                                        "paid").c_str()
                                ),
                                System::Globalization::CultureInfo::InvariantCulture
                            );
                    }

                    liveBalance =
                        chargeAmount - paidAmount;
                }

                if (liveBalance <= Decimal(0))
                {
                    throw gcnew Exception(
                        L"The selected fee charge has already been fully paid."
                    );
                }

                if (amount > liveBalance)
                {
                    throw gcnew Exception(
                        L"The payment amount is greater than the current outstanding balance."
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
                    liveBalance - amount;

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
                transactionStarted = false;

                MessageBox::Show(
                    paymentForm,
                    L"Payment recorded successfully.\n\n"
                    L"Receipt: " + receipt +
                    L"\nStudent: " + student->Name +
                    L"\nAmount: UGX " +
                    amount.ToString(
                        L"N2",
                        System::Globalization::CultureInfo::InvariantCulture) +
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
                if (transactionStarted && con != nullptr)
                {
                    try
                    {
                        con->rollback();
                        con->setAutoCommit(true);
                    }
                    catch (Exception^)
                    {
                    }
                }

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
                if (transactionStarted && con != nullptr)
                {
                    try
                    {
                        con->rollback();
                        con->setAutoCommit(true);
                    }
                    catch (Exception^)
                    {
                    }
                }

                MessageBox::Show(
                    paymentForm,
                    ex->Message,
                    L"Unable to Record Payment",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }

        Panel^ CreatePaymentFieldBlock(String^ labelText, Control^ input)
        {
            Panel^ field =
                gcnew Panel();

            field->Dock = DockStyle::Fill;
            field->Padding =
                System::Windows::Forms::Padding(10, 8, 10, 8);

            Label^ label =
                CreateFieldLabel(labelText);

            label->Dock = DockStyle::Top;
            label->Height = 24;
            label->Margin =
                System::Windows::Forms::Padding(0);

            input->Dock = DockStyle::Fill;
            input->Margin =
                System::Windows::Forms::Padding(0);

            field->Controls->Add(input);
            field->Controls->Add(label);

            return field;
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
            paymentForm->ClientSize = Drawing::Size(760, 620);
            paymentForm->MinimumSize = Drawing::Size(720, 580);
            paymentForm->BackColor = ThemeManager::Canvas();

            TableLayoutPanel^ root =
                gcnew TableLayoutPanel();

            root->Dock = DockStyle::Fill;
            root->Padding =
                System::Windows::Forms::Padding(22);
            root->ColumnCount = 1;
            root->RowCount = 6;

            root->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Percent, 100.0F));

            root->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 52.0F));
            root->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 30.0F));
            root->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 326.0F));
            root->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 42.0F));
            root->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 76.0F));
            root->RowStyles->Add(
                gcnew RowStyle(SizeType::Percent, 100.0F));

            Label^ title =
                CreateLabel(
                    L"Record Payment",
                    FinanceTheme::Dialog,
                    FinanceTheme::TextStrong);
            title->Dock = DockStyle::Fill;
            title->AutoEllipsis = false;
            title->TextAlign = ContentAlignment::MiddleLeft;

            paymentStudentInfoLabel =
                CreateLabel(
                    L"Select a student to view outstanding charges.",
                    FinanceTheme::Small,
                    FinanceTheme::TextMuted);
            paymentStudentInfoLabel->Dock = DockStyle::Fill;
            paymentStudentInfoLabel->AutoEllipsis = false;
            paymentStudentInfoLabel->TextAlign =
                ContentAlignment::MiddleLeft;

            TableLayoutPanel^ details =
                gcnew TableLayoutPanel();

            details->Dock = DockStyle::Fill;
            details->Padding =
                System::Windows::Forms::Padding(12);
            details->ColumnCount = 2;
            details->RowCount = 4;
            details->BackColor = Color::White;
            details->BorderStyle = BorderStyle::FixedSingle;

            details->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Percent, 50.0F));
            details->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Percent, 50.0F));

            for (int i = 0; i < 4; ++i)
            {
                details->RowStyles->Add(
                    gcnew RowStyle(SizeType::Percent, 25.0F));
            }

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

            details->Controls->Add(
                CreatePaymentFieldBlock(
                    L"Student",
                    paymentStudentBox),
                0, 0);

            details->Controls->Add(
                CreatePaymentFieldBlock(
                    L"Fee Charge",
                    paymentChargeBox),
                1, 0);

            details->Controls->Add(
                CreatePaymentFieldBlock(
                    L"Amount (UGX)",
                    paymentAmountBox),
                0, 1);

            details->Controls->Add(
                CreatePaymentFieldBlock(
                    L"Payment Date",
                    paymentDatePicker),
                1, 1);

            details->Controls->Add(
                CreatePaymentFieldBlock(
                    L"Payment Method",
                    paymentMethodBox),
                0, 2);

            details->Controls->Add(
                CreatePaymentFieldBlock(
                    L"Provider",
                    paymentProviderBox),
                1, 2);

            details->Controls->Add(
                CreatePaymentFieldBlock(
                    L"Transaction Reference",
                    paymentReferenceBox),
                0, 3);

            details->Controls->Add(
                CreatePaymentFieldBlock(
                    L"Payer Contact",
                    paymentPayerContactBox),
                1, 3);

            Panel^ balancePanel =
                gcnew Panel();

            balancePanel->Dock = DockStyle::Fill;
            balancePanel->Padding =
                System::Windows::Forms::Padding(14, 5, 14, 5);
            balancePanel->BackColor =
                FinanceTheme::Surface;
            balancePanel->BorderStyle =
                BorderStyle::FixedSingle;

            paymentBalanceLabel =
                CreateLabel(
                    L"Outstanding: UGX 0.00",
                    FinanceTheme::Section,
                    FinanceTheme::TextStrong);

            paymentBalanceLabel->Dock =
                DockStyle::Fill;
            paymentBalanceLabel->AutoEllipsis = false;
            paymentBalanceLabel->TextAlign =
                ContentAlignment::MiddleLeft;

            balancePanel->Controls->Add(
                paymentBalanceLabel);

            Panel^ remarksPanel =
                gcnew Panel();

            remarksPanel->Dock = DockStyle::Fill;
            remarksPanel->Padding =
                System::Windows::Forms::Padding(10, 4, 10, 4);

            Label^ remarksLabel =
                CreateFieldLabel(L"Remarks");

            remarksLabel->Dock = DockStyle::Top;
            remarksLabel->Height = 22;
            remarksLabel->Margin =
                System::Windows::Forms::Padding(0);

            paymentRemarksBox = gcnew TextBox();
            paymentRemarksBox->Dock = DockStyle::Fill;
            paymentRemarksBox->Multiline = true;
            paymentRemarksBox->ScrollBars =
                ScrollBars::Vertical;
            paymentRemarksBox->MaxLength = 255;
            paymentRemarksBox->Margin =
                System::Windows::Forms::Padding(0);

            remarksPanel->Controls->Add(
                paymentRemarksBox);
            remarksPanel->Controls->Add(
                remarksLabel);

            FlowLayoutPanel^ footer =
                gcnew FlowLayoutPanel();

            footer->Dock = DockStyle::Fill;
            footer->FlowDirection =
                FlowDirection::RightToLeft;
            footer->WrapContents = false;
            footer->Padding =
                System::Windows::Forms::Padding(0, 8, 0, 0);

            Button^ cancel =
                gcnew Button();

            cancel->Text = L"Cancel";
            cancel->Width = 100;
            cancel->Height = 34;
            cancel->DialogResult =
                System::Windows::Forms::DialogResult::Cancel;

            paymentSaveButton =
                gcnew Button();

            paymentSaveButton->Text =
                L"Record Payment";
            paymentSaveButton->Width = 145;
            paymentSaveButton->Height = 34;

            footer->Controls->Add(cancel);
            footer->Controls->Add(
                paymentSaveButton);

            root->Controls->Add(
                title,
                0, 0);

            root->Controls->Add(
                paymentStudentInfoLabel,
                0, 1);

            root->Controls->Add(
                details,
                0, 2);

            root->Controls->Add(
                balancePanel,
                0, 3);

            root->Controls->Add(
                remarksPanel,
                0, 4);

            root->Controls->Add(
                footer,
                0, 5);

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

            ThemeManager::ApplyToForm(
                paymentForm);

            LoadPaymentProviders();
            LoadPaymentStudents();

            paymentForm->AcceptButton =
                paymentSaveButton;
            paymentForm->CancelButton =
                cancel;

            paymentForm->ShowDialog(this);

            delete paymentForm;
            paymentForm = nullptr;
        }

        void LoadPaymentHistory()
        {
            if (paymentHistoryGrid == nullptr)
                return;

            paymentHistoryGrid->Rows->Clear();

            String^ search =
                paymentHistorySearchBox == nullptr
                ? L""
                : paymentHistorySearchBox->Text->Trim();

            String^ method =
                paymentHistoryMethodBox == nullptr ||
                paymentHistoryMethodBox->SelectedIndex <= 0
                ? L""
                : paymentHistoryMethodBox->Text->Trim();

            String^ status =
                paymentHistoryStatusBox == nullptr ||
                paymentHistoryStatusBox->SelectedIndex <= 0
                ? L""
                : paymentHistoryStatusBox->Text->Trim();

            DateTime fromDate =
                paymentHistoryFromPicker->Value.Date;

            DateTime toDate =
                paymentHistoryToPicker->Value.Date;

            if (toDate < fromDate)
            {
                paymentHistorySummaryLabel->Text =
                    L"Select a valid date range.";
                return;
            }

            try
            {
                auto con =
                    DbConnection::GetConnection();

                String^ sqlText =
                    L"SELECT "
                    L"p.payment_id, "
                    L"p.receipt_number, "
                    L"p.payment_date, "
                    L"p.amount, "
                    L"COALESCE(pm.method_name, p.payment_method, '-') AS method_name, "
                    L"COALESCE(pp.provider_name, '-') AS provider_name, "
                    L"COALESCE(p.transaction_reference, '-') AS transaction_reference, "
                    L"CONCAT_WS(' ', s.first_name, s.middle_name, s.last_name) AS student_name, "
                    L"s.registration_number, "
                    L"COALESCE(p.payer_contact, '-') AS payer_contact, "
                    L"COALESCE(p.payment_status, 'Confirmed') AS payment_status, "
                    L"COALESCE(u.full_name, u.username, '-') AS received_by, "
                    L"COALESCE(GROUP_CONCAT(DISTINCT fs.fee_name ORDER BY fs.fee_name SEPARATOR ', '), '-') AS fee_items "
                    L"FROM payments p "
                    L"INNER JOIN students s ON s.student_id = p.student_id "
                    L"LEFT JOIN payment_methods pm ON pm.payment_method_id = p.payment_method_id "
                    L"LEFT JOIN payment_providers pp ON pp.payment_provider_id = p.payment_provider_id "
                    L"LEFT JOIN users u ON u.user_id = p.received_by "
                    L"LEFT JOIN payment_allocations pa ON pa.payment_id = p.payment_id "
                    L"LEFT JOIN fee_charges fc ON fc.fee_charge_id = pa.fee_charge_id "
                    L"LEFT JOIN fee_structures fs ON fs.fee_structure_id = fc.fee_structure_id "
                    L"WHERE p.payment_date BETWEEN ? AND ? ";

                if (!String::IsNullOrWhiteSpace(search))
                {
                    sqlText +=
                        L"AND ("
                        L"LOWER(s.registration_number) LIKE LOWER(?) "
                        L"OR LOWER(CONCAT_WS(' ', s.first_name, s.middle_name, s.last_name)) LIKE LOWER(?) "
                        L"OR LOWER(p.receipt_number) LIKE LOWER(?) "
                        L"OR LOWER(COALESCE(p.transaction_reference, '')) LIKE LOWER(?) "
                        L"OR LOWER(COALESCE(pp.provider_name, '')) LIKE LOWER(?)"
                        L") ";
                }

                if (!String::IsNullOrWhiteSpace(method))
                    sqlText +=
                        L"AND COALESCE(pm.method_name, p.payment_method) = ? ";

                if (!String::IsNullOrWhiteSpace(status))
                    sqlText +=
                        L"AND COALESCE(p.payment_status, 'Confirmed') = ? ";

                sqlText +=
                    L"GROUP BY "
                    L"p.payment_id, p.receipt_number, p.payment_date, p.amount, "
                    L"pm.method_name, p.payment_method, pp.provider_name, "
                    L"p.transaction_reference, s.student_id, s.registration_number, "
                    L"s.first_name, s.middle_name, s.last_name, p.payer_contact, "
                    L"p.payment_status, u.full_name, u.username "
                    L"ORDER BY p.payment_date DESC, p.payment_id DESC";

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        msclr::interop::marshal_as<std::string>(
                            sqlText
                        )
                    )
                );

                int parameter = 1;

                stmt->setString(
                    parameter++,
                    msclr::interop::marshal_as<std::string>(
                        fromDate.ToString(L"yyyy-MM-dd")
                    )
                );

                stmt->setString(
                    parameter++,
                    msclr::interop::marshal_as<std::string>(
                        toDate.ToString(L"yyyy-MM-dd")
                    )
                );

                if (!String::IsNullOrWhiteSpace(search))
                {
                    String^ likeValue =
                        L"%" + search + L"%";

                    for (int i = 0; i < 5; ++i)
                    {
                        stmt->setString(
                            parameter++,
                            msclr::interop::marshal_as<std::string>(
                                likeValue
                            )
                        );
                    }
                }

                if (!String::IsNullOrWhiteSpace(method))
                {
                    stmt->setString(
                        parameter++,
                        msclr::interop::marshal_as<std::string>(
                            method
                        )
                    );
                }

                if (!String::IsNullOrWhiteSpace(status))
                {
                    stmt->setString(
                        parameter++,
                        msclr::interop::marshal_as<std::string>(
                            status
                        )
                    );
                }

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                Decimal total =
                    Decimal(0);

                int count = 0;

                while (result->next())
                {
                    String^ paymentDate =
                        gcnew String(
                            result->getString(
                                "payment_date").c_str()
                        );

                    Decimal amount =
                        Decimal::Parse(
                            gcnew String(
                                result->getString(
                                    "amount").c_str()
                            ),
                            System::Globalization::CultureInfo::InvariantCulture
                        );

                    paymentHistoryGrid->Rows->Add(
                        result->getInt("payment_id"),
                        gcnew String(
                            result->getString(
                                "receipt_number").c_str()),
                        gcnew String(
                            result->getString(
                                "registration_number").c_str()),
                        gcnew String(
                            result->getString(
                                "student_name").c_str()),
                        paymentDate,
                        amount.ToString(
                            L"N2",
                            System::Globalization::CultureInfo::InvariantCulture
                        ),
                        gcnew String(
                            result->getString(
                                "method_name").c_str()),
                        gcnew String(
                            result->getString(
                                "provider_name").c_str()),
                        gcnew String(
                            result->getString(
                                "transaction_reference").c_str()),
                        gcnew String(
                            result->getString(
                                "payment_status").c_str())
                    );

                    total += amount;
                    ++count;
                }

                paymentHistorySummaryLabel->Text =
                    count.ToString() +
                    L" payment(s)  |  Total: UGX " +
                    total.ToString(
                        L"N2",
                        System::Globalization::CultureInfo::InvariantCulture
                    );
            }
            catch (sql::SQLException& ex)
            {
                paymentHistorySummaryLabel->Text =
                    L"Unable to load payment history.";

                MessageBox::Show(
                    paymentHistoryForm,
                    gcnew String(ex.what()),
                    L"Payment History",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }

        void PaymentHistoryFilterChanged(
            Object^ sender,
            EventArgs^ e)
        {
            LoadPaymentHistory();
        }

        void PaymentHistorySearchKeyDown(
            Object^ sender,
            KeyEventArgs^ e)
        {
            if (e == nullptr)
                return;

            if (e->KeyCode == Keys::Enter)
            {
                LoadPaymentHistory();
                e->SuppressKeyPress = true;
                e->Handled = true;
            }
        }

        void PaymentHistoryViewClick(
            Object^ sender,
            EventArgs^ e)
        {
            ShowSelectedPaymentDetails();
        }

        void PaymentHistoryGridDoubleClick(
            Object^ sender,
            DataGridViewCellEventArgs^ e)
        {
            if (e != nullptr && e->RowIndex >= 0)
                ShowSelectedPaymentDetails();
        }

        void ShowSelectedPaymentDetails()
        {
            if (paymentHistoryGrid == nullptr ||
                paymentHistoryGrid->SelectedRows->Count == 0)
            {
                MessageBox::Show(
                    paymentHistoryForm,
                    L"Select a payment first.",
                    L"Payment History",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information
                );
                return;
            }

            DataGridViewRow^ row =
                paymentHistoryGrid->SelectedRows[0];

            String^ receipt =
                Convert::ToString(
                    row->Cells[L"Receipt"]->Value);

            String^ student =
                Convert::ToString(
                    row->Cells[L"Student"]->Value);

            String^ registration =
                Convert::ToString(
                    row->Cells[L"Registration"]->Value);

            String^ amount =
                Convert::ToString(
                    row->Cells[L"Amount"]->Value);

            String^ date =
                Convert::ToString(
                    row->Cells[L"PaymentDate"]->Value);

            String^ method =
                Convert::ToString(
                    row->Cells[L"Method"]->Value);

            String^ provider =
                Convert::ToString(
                    row->Cells[L"Provider"]->Value);

            String^ reference =
                Convert::ToString(
                    row->Cells[L"Reference"]->Value);

            String^ status =
                Convert::ToString(
                    row->Cells[L"Status"]->Value);

            Form^ details =
                gcnew Form();

            details->Text =
                L"SchoolCore - Payment Details";
            details->StartPosition =
                FormStartPosition::CenterParent;
            details->FormBorderStyle =
                System::Windows::Forms::FormBorderStyle::FixedDialog;
            details->MaximizeBox = false;
            details->MinimizeBox = false;
            details->ShowInTaskbar = false;
            details->ClientSize =
                Drawing::Size(600, 470);
            details->BackColor =
                ThemeManager::Canvas();

            TableLayoutPanel^ layout =
                gcnew TableLayoutPanel();

            layout->Dock = DockStyle::Fill;
            layout->Padding =
                System::Windows::Forms::Padding(28);
            layout->ColumnCount = 2;
            layout->RowCount = 10;

            layout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Absolute,
                    150.0F));
            layout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    100.0F));

            for (int i = 0; i < 9; ++i)
            {
                layout->RowStyles->Add(
                    gcnew RowStyle(
                        SizeType::Absolute,
                        36.0F));
            }

            layout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Percent,
                    100.0F));

            Label^ title =
                CreateLabel(
                    L"Payment Details",
                    FinanceTheme::Dialog,
                    FinanceTheme::TextStrong);

            title->Dock = DockStyle::Fill;
            layout->Controls->Add(
                title,
                0,
                0);
            layout->SetColumnSpan(
                title,
                2);

            array<String^>^ labels =
                gcnew array<String^>
                {
                    L"Receipt",
                    L"Registration",
                    L"Student",
                    L"Payment Date",
                    L"Amount",
                    L"Method",
                    L"Provider",
                    L"Reference",
                    L"Status"
                };

            array<String^>^ values =
                gcnew array<String^>
                {
                    receipt,
                    registration,
                    student,
                    date,
                    L"UGX " + amount,
                    method,
                    provider,
                    reference,
                    status
                };

            for (int i = 0; i < labels->Length; ++i)
            {
                Label^ caption =
                    CreateLabel(
                        labels[i],
                        FinanceTheme::Body,
                        FinanceTheme::TextMuted);

                caption->Dock = DockStyle::Fill;

                Label^ value =
                    CreateLabel(
                        values[i],
                        FinanceTheme::Body,
                        FinanceTheme::TextStrong);

                value->Dock = DockStyle::Fill;
                value->AutoEllipsis = true;

                layout->Controls->Add(
                    caption,
                    0,
                    i + 1);
                layout->Controls->Add(
                    value,
                    1,
                    i + 1);
            }

            Button^ close =
                gcnew Button();

            close->Text = L"Close";
            close->Width = 110;
            close->DialogResult =
                System::Windows::Forms::DialogResult::Cancel;

            FlowLayoutPanel^ footer =
                gcnew FlowLayoutPanel();

            footer->Dock = DockStyle::Fill;
            footer->FlowDirection =
                FlowDirection::RightToLeft;
            footer->WrapContents = false;
            footer->Controls->Add(close);

            layout->Controls->Add(
                footer,
                0,
                9);
            layout->SetColumnSpan(
                footer,
                2);

            details->Controls->Add(layout);
            details->CancelButton = close;

            ThemeManager::ApplyToForm(details);

            details->ShowDialog(
                paymentHistoryForm);

            delete details;
        }

        void OpenPaymentHistoryDialog()
        {
            if (!AuthSession::HasPermission(PermView))
            {
                MessageBox::Show(
                    this,
                    L"You do not have permission to view payment history.",
                    L"SchoolCore - Access denied",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                return;
            }

            paymentHistoryForm =
                gcnew Form();

            paymentHistoryForm->Text =
                L"SchoolCore - Payment History";
            paymentHistoryForm->StartPosition =
                FormStartPosition::CenterParent;
            paymentHistoryForm->FormBorderStyle =
                System::Windows::Forms::FormBorderStyle::Sizable;
            paymentHistoryForm->MinimizeBox = false;
            paymentHistoryForm->MaximizeBox = true;
            paymentHistoryForm->ShowInTaskbar = false;
            paymentHistoryForm->ClientSize =
                Drawing::Size(1120, 680);
            paymentHistoryForm->MinimumSize =
                Drawing::Size(980, 580);
            paymentHistoryForm->BackColor =
                ThemeManager::Canvas();

            TableLayoutPanel^ root =
                gcnew TableLayoutPanel();

            root->Dock = DockStyle::Fill;
            root->Padding =
                System::Windows::Forms::Padding(24);
            root->ColumnCount = 1;
            root->RowCount = 4;

            root->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    46.0F));

            root->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    78.0F));

            root->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Percent,
                    100.0F));

            root->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    50.0F));

            Label^ title =
                CreateLabel(
                    L"Payment History",
                    FinanceTheme::Dialog,
                    FinanceTheme::TextStrong);

            title->Dock = DockStyle::Fill;

            FlowLayoutPanel^ filters =
                gcnew FlowLayoutPanel();

            filters->Dock = DockStyle::Fill;
            filters->WrapContents = false;
            filters->AutoScroll = true;
            filters->Padding =
                System::Windows::Forms::Padding(0, 8, 0, 4);

            Label^ searchLabel =
                CreateLabel(
                    L"Search",
                    FinanceTheme::Small,
                    FinanceTheme::TextStrong);

            searchLabel->Width = 48;
            searchLabel->TextAlign =
                ContentAlignment::MiddleLeft;

            paymentHistorySearchBox =
                gcnew TextBox();

            paymentHistorySearchBox->Width = 185;

            Label^ methodLabel =
                CreateLabel(
                    L"Method",
                    FinanceTheme::Small,
                    FinanceTheme::TextStrong);

            methodLabel->Width = 52;
            methodLabel->TextAlign =
                ContentAlignment::MiddleLeft;

            paymentHistoryMethodBox =
                gcnew ComboBox();

            paymentHistoryMethodBox->Width = 160;
            paymentHistoryMethodBox->DropDownStyle =
                ComboBoxStyle::DropDownList;
            paymentHistoryMethodBox->Items->Add(
                L"All Methods");
            paymentHistoryMethodBox->Items->Add(
                L"Mobile Money");
            paymentHistoryMethodBox->Items->Add(
                L"Bank");
            paymentHistoryMethodBox->Items->Add(
                L"Online/Electronic Payment");
            paymentHistoryMethodBox->SelectedIndex = 0;

            Label^ statusLabel =
                CreateLabel(
                    L"Status",
                    FinanceTheme::Small,
                    FinanceTheme::TextStrong);

            statusLabel->Width = 44;
            statusLabel->TextAlign =
                ContentAlignment::MiddleLeft;

            paymentHistoryStatusBox =
                gcnew ComboBox();

            paymentHistoryStatusBox->Width = 120;
            paymentHistoryStatusBox->DropDownStyle =
                ComboBoxStyle::DropDownList;
            paymentHistoryStatusBox->Items->Add(
                L"All Statuses");
            paymentHistoryStatusBox->Items->Add(
                L"Confirmed");
            paymentHistoryStatusBox->Items->Add(
                L"Pending");
            paymentHistoryStatusBox->Items->Add(
                L"Reversed");
            paymentHistoryStatusBox->Items->Add(
                L"Cancelled");
            paymentHistoryStatusBox->SelectedIndex = 0;

            Label^ fromLabel =
                CreateLabel(
                    L"From",
                    FinanceTheme::Small,
                    FinanceTheme::TextStrong);

            fromLabel->Width = 35;
            fromLabel->TextAlign =
                ContentAlignment::MiddleLeft;

            paymentHistoryFromPicker =
                gcnew DateTimePicker();

            paymentHistoryFromPicker->Width = 105;
            paymentHistoryFromPicker->Format =
                DateTimePickerFormat::Short;
            paymentHistoryFromPicker->Value =
                DateTime::Today.AddMonths(-1);

            Label^ toLabel =
                CreateLabel(
                    L"To",
                    FinanceTheme::Small,
                    FinanceTheme::TextStrong);

            toLabel->Width = 24;
            toLabel->TextAlign =
                ContentAlignment::MiddleLeft;

            paymentHistoryToPicker =
                gcnew DateTimePicker();

            paymentHistoryToPicker->Width = 105;
            paymentHistoryToPicker->Format =
                DateTimePickerFormat::Short;
            paymentHistoryToPicker->Value =
                DateTime::Today;

            Button^ searchButton =
                gcnew Button();

            searchButton->Text = L"Refresh";
            searchButton->Width = 90;
            searchButton->Height = 30;

            filters->Controls->Add(searchLabel);
            filters->Controls->Add(paymentHistorySearchBox);
            filters->Controls->Add(methodLabel);
            filters->Controls->Add(paymentHistoryMethodBox);
            filters->Controls->Add(statusLabel);
            filters->Controls->Add(paymentHistoryStatusBox);
            filters->Controls->Add(fromLabel);
            filters->Controls->Add(paymentHistoryFromPicker);
            filters->Controls->Add(toLabel);
            filters->Controls->Add(paymentHistoryToPicker);
            filters->Controls->Add(searchButton);

            paymentHistoryGrid =
                gcnew DataGridView();

            paymentHistoryGrid->Dock =
                DockStyle::Fill;
            paymentHistoryGrid->AllowUserToAddRows = false;
            paymentHistoryGrid->AllowUserToDeleteRows = false;
            paymentHistoryGrid->AllowUserToResizeRows = false;
            paymentHistoryGrid->ReadOnly = true;
            paymentHistoryGrid->MultiSelect = false;
            paymentHistoryGrid->SelectionMode =
                DataGridViewSelectionMode::FullRowSelect;
            paymentHistoryGrid->AutoGenerateColumns = false;
            paymentHistoryGrid->AutoSizeRowsMode =
                DataGridViewAutoSizeRowsMode::None;
            paymentHistoryGrid->RowHeadersVisible = false;

            DataGridViewTextBoxColumn^ idColumn =
                gcnew DataGridViewTextBoxColumn();
            idColumn->Name = L"PaymentId";
            idColumn->HeaderText = L"ID";
            idColumn->Width = 50;

            DataGridViewTextBoxColumn^ receiptColumn =
                gcnew DataGridViewTextBoxColumn();
            receiptColumn->Name = L"Receipt";
            receiptColumn->HeaderText = L"Receipt";
            receiptColumn->Width = 135;

            DataGridViewTextBoxColumn^ registrationColumn =
                gcnew DataGridViewTextBoxColumn();
            registrationColumn->Name = L"Registration";
            registrationColumn->HeaderText = L"Registration";
            registrationColumn->Width = 120;

            DataGridViewTextBoxColumn^ studentColumn =
                gcnew DataGridViewTextBoxColumn();
            studentColumn->Name = L"Student";
            studentColumn->HeaderText = L"Student";
            studentColumn->Width = 180;

            DataGridViewTextBoxColumn^ dateColumn =
                gcnew DataGridViewTextBoxColumn();
            dateColumn->Name = L"PaymentDate";
            dateColumn->HeaderText = L"Date";
            dateColumn->Width = 95;

            DataGridViewTextBoxColumn^ amountColumn =
                gcnew DataGridViewTextBoxColumn();
            amountColumn->Name = L"Amount";
            amountColumn->HeaderText = L"Amount (UGX)";
            amountColumn->Width = 110;

            DataGridViewTextBoxColumn^ methodColumn =
                gcnew DataGridViewTextBoxColumn();
            methodColumn->Name = L"Method";
            methodColumn->HeaderText = L"Method";
            methodColumn->Width = 145;

            DataGridViewTextBoxColumn^ providerColumn =
                gcnew DataGridViewTextBoxColumn();
            providerColumn->Name = L"Provider";
            providerColumn->HeaderText = L"Provider";
            providerColumn->Width = 145;

            DataGridViewTextBoxColumn^ referenceColumn =
                gcnew DataGridViewTextBoxColumn();
            referenceColumn->Name = L"Reference";
            referenceColumn->HeaderText = L"Transaction Reference";
            referenceColumn->Width = 170;

            DataGridViewTextBoxColumn^ statusColumn =
                gcnew DataGridViewTextBoxColumn();
            statusColumn->Name = L"Status";
            statusColumn->HeaderText = L"Status";
            statusColumn->Width = 100;

            paymentHistoryGrid->Columns->Add(idColumn);
            paymentHistoryGrid->Columns->Add(receiptColumn);
            paymentHistoryGrid->Columns->Add(registrationColumn);
            paymentHistoryGrid->Columns->Add(studentColumn);
            paymentHistoryGrid->Columns->Add(dateColumn);
            paymentHistoryGrid->Columns->Add(amountColumn);
            paymentHistoryGrid->Columns->Add(methodColumn);
            paymentHistoryGrid->Columns->Add(providerColumn);
            paymentHistoryGrid->Columns->Add(referenceColumn);
            paymentHistoryGrid->Columns->Add(statusColumn);

            FlowLayoutPanel^ footer =
                gcnew FlowLayoutPanel();

            footer->Dock = DockStyle::Fill;
            footer->FlowDirection =
                FlowDirection::RightToLeft;
            footer->WrapContents = false;

            Button^ close =
                gcnew Button();

            close->Text = L"Close";
            close->Width = 100;
            close->DialogResult =
                System::Windows::Forms::DialogResult::Cancel;

            paymentHistoryViewButton =
                gcnew Button();

            paymentHistoryViewButton->Text =
                L"View Details";
            paymentHistoryViewButton->Width = 120;

            paymentHistorySummaryLabel =
                CreateLabel(
                    L"0 payment(s)",
                    FinanceTheme::Small,
                    FinanceTheme::TextMuted);

            paymentHistorySummaryLabel->AutoSize = true;
            paymentHistorySummaryLabel->Margin =
                System::Windows::Forms::Padding(
                    0,
                    9,
                    18,
                    0);

            footer->Controls->Add(close);
            footer->Controls->Add(
                paymentHistoryViewButton);
            footer->Controls->Add(
                paymentHistorySummaryLabel);

            root->Controls->Add(title, 0, 0);
            root->Controls->Add(filters, 0, 1);
            root->Controls->Add(
                paymentHistoryGrid,
                0,
                2);
            root->Controls->Add(footer, 0, 3);

            searchButton->Click +=
                gcnew EventHandler(
                    this,
                    &Finance::PaymentHistoryFilterChanged
                );

            paymentHistorySearchBox->KeyDown +=
                gcnew KeyEventHandler(
                    this,
                    &Finance::PaymentHistorySearchKeyDown
                );

            paymentHistoryMethodBox->SelectedIndexChanged +=
                gcnew EventHandler(
                    this,
                    &Finance::PaymentHistoryFilterChanged
                );

            paymentHistoryStatusBox->SelectedIndexChanged +=
                gcnew EventHandler(
                    this,
                    &Finance::PaymentHistoryFilterChanged
                );

            paymentHistoryFromPicker->ValueChanged +=
                gcnew EventHandler(
                    this,
                    &Finance::PaymentHistoryFilterChanged
                );

            paymentHistoryToPicker->ValueChanged +=
                gcnew EventHandler(
                    this,
                    &Finance::PaymentHistoryFilterChanged
                );

            paymentHistoryViewButton->Click +=
                gcnew EventHandler(
                    this,
                    &Finance::PaymentHistoryViewClick
                );

            paymentHistoryGrid->CellDoubleClick +=
                gcnew DataGridViewCellEventHandler(
                    this,
                    &Finance::PaymentHistoryGridDoubleClick
                );

            paymentHistoryForm->Controls->Add(root);
            paymentHistoryForm->AcceptButton = searchButton;
            paymentHistoryForm->CancelButton = close;

            ThemeManager::ApplyToForm(
                paymentHistoryForm);

            LoadPaymentHistory();

            paymentHistoryForm->ShowDialog(this);

            delete paymentHistoryForm;
            paymentHistoryForm = nullptr;
            paymentHistoryGrid = nullptr;
            paymentHistorySearchBox = nullptr;
            paymentHistoryMethodBox = nullptr;
            paymentHistoryStatusBox = nullptr;
            paymentHistoryFromPicker = nullptr;
            paymentHistoryToPicker = nullptr;
            paymentHistorySummaryLabel = nullptr;
            paymentHistoryViewButton = nullptr;
        }

        void LoadFeeStructureYears()
        {
            feeStructureYearBox->Items->Clear();

            try
            {
                auto con = DbConnection::GetConnection();
                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT academic_year_id, year_name "
                        "FROM academic_years "
                        "ORDER BY start_date DESC, academic_year_id DESC"
                    )
                );
                std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());

                while (result->next())
                {
                    feeStructureYearBox->Items->Add(
                        gcnew FinanceLookupItem(
                            result->getInt("academic_year_id"),
                            gcnew String(result->getString("year_name").c_str())
                        )
                    );
                }

                if (feeStructureYearBox->Items->Count > 0)
                    feeStructureYearBox->SelectedIndex = 0;
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    feeStructureEditorForm,
                    gcnew String(ex.what()),
                    L"Unable to Load Academic Years",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }

        void FeeStructureYearChanged(Object^ sender, EventArgs^ e)
        {
            FinanceLookupItem^ year =
                dynamic_cast<FinanceLookupItem^>(
                    feeStructureYearBox->SelectedItem
                );

            feeStructureTermBox->Items->Clear();

            if (year == nullptr || year->Id <= 0)
                return;

            try
            {
                auto con = DbConnection::GetConnection();
                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT term_id, term_name "
                        "FROM terms "
                        "WHERE academic_year_id = ? "
                        "ORDER BY term_id"
                    )
                );
                stmt->setInt(1, year->Id);

                std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());

                while (result->next())
                {
                    feeStructureTermBox->Items->Add(
                        gcnew FinanceLookupItem(
                            result->getInt("term_id"),
                            gcnew String(result->getString("term_name").c_str())
                        )
                    );
                }

                if (feeStructureTermBox->Items->Count > 0)
                    feeStructureTermBox->SelectedIndex = 0;
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    feeStructureEditorForm,
                    gcnew String(ex.what()),
                    L"Unable to Load Terms",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }

        void LoadFeeStructureClasses()
        {
            feeStructureClassBox->Items->Clear();

            try
            {
                auto con = DbConnection::GetConnection();
                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT class_id, class_name "
                        "FROM classes "
                        "ORDER BY class_name"
                    )
                );
                std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());

                while (result->next())
                {
                    feeStructureClassBox->Items->Add(
                        gcnew FinanceLookupItem(
                            result->getInt("class_id"),
                            gcnew String(result->getString("class_name").c_str())
                        )
                    );
                }

                if (feeStructureClassBox->Items->Count > 0)
                    feeStructureClassBox->SelectedIndex = 0;
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    feeStructureEditorForm,
                    gcnew String(ex.what()),
                    L"Unable to Load Classes",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }

        void FeeStructureClassChanged(Object^ sender, EventArgs^ e)
        {
            feeStructureStreamBox->Items->Clear();
            feeStructureStreamBox->Items->Add(
                gcnew FinanceLookupItem(0, L"All Streams")
            );

            FinanceLookupItem^ classItem =
                dynamic_cast<FinanceLookupItem^>(
                    feeStructureClassBox->SelectedItem
                );

            if (classItem == nullptr || classItem->Id <= 0)
            {
                feeStructureStreamBox->SelectedIndex = 0;
                return;
            }

            try
            {
                auto con = DbConnection::GetConnection();
                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT stream_id, stream_name "
                        "FROM streams "
                        "WHERE class_id = ? "
                        "ORDER BY stream_name"
                    )
                );
                stmt->setInt(1, classItem->Id);

                std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());

                while (result->next())
                {
                    feeStructureStreamBox->Items->Add(
                        gcnew FinanceLookupItem(
                            result->getInt("stream_id"),
                            gcnew String(result->getString("stream_name").c_str())
                        )
                    );
                }

                feeStructureStreamBox->SelectedIndex = 0;
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    feeStructureEditorForm,
                    gcnew String(ex.what()),
                    L"Unable to Load Streams",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }

        void SaveFeeStructure(Object^ sender, EventArgs^ e)
        {
            FinanceLookupItem^ year =
                dynamic_cast<FinanceLookupItem^>(feeStructureYearBox->SelectedItem);
            FinanceLookupItem^ term =
                dynamic_cast<FinanceLookupItem^>(feeStructureTermBox->SelectedItem);
            FinanceLookupItem^ classItem =
                dynamic_cast<FinanceLookupItem^>(feeStructureClassBox->SelectedItem);
            FinanceLookupItem^ streamItem =
                dynamic_cast<FinanceLookupItem^>(feeStructureStreamBox->SelectedItem);

            String^ feeName = feeStructureNameBox->Text->Trim();
            Decimal amount;

            if (year == nullptr || term == nullptr || classItem == nullptr)
            {
                MessageBox::Show(
                    feeStructureEditorForm,
                    L"Select the academic year, term and class.",
                    L"Fee Structure",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                return;
            }

            if (String::IsNullOrWhiteSpace(feeName))
            {
                MessageBox::Show(
                    feeStructureEditorForm,
                    L"Enter the fee name.",
                    L"Fee Structure",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                return;
            }

            try
            {
                amount =
                    Decimal::Parse(
                        feeStructureAmountBox->Text->Trim(),
                        Globalization::NumberStyles::Number,
                        System::Globalization::CultureInfo::InvariantCulture
                    );
            }
            catch (Exception^)
            {
                MessageBox::Show(
                    feeStructureEditorForm,
                    L"Enter a valid fee amount.",
                    L"Fee Structure",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                return;
            }

            if (amount <= Decimal(0))
            {
                MessageBox::Show(
                    feeStructureEditorForm,
                    L"Fee amount must be greater than zero.",
                    L"Fee Structure",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                return;
            }

            try
            {
                auto con = DbConnection::GetConnection();

                String^ insertSql;

                if (streamItem != nullptr && streamItem->Id > 0)
                {
                    insertSql =
                        L"INSERT INTO fee_structures "
                        L"(academic_year_id, term_id, class_id, stream_id, "
                        L"fee_name, amount, due_date, description, status) "
                        L"VALUES (?, ?, ?, ?, ?, ?, ?, ?, 'Active')";
                }
                else
                {
                    insertSql =
                        L"INSERT INTO fee_structures "
                        L"(academic_year_id, term_id, class_id, stream_id, "
                        L"fee_name, amount, due_date, description, status) "
                        L"VALUES (?, ?, ?, NULL, ?, ?, ?, ?, 'Active')";
                }

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        msclr::interop::marshal_as<std::string>(insertSql)
                    )
                );

                int p = 1;
                stmt->setInt(p++, year->Id);
                stmt->setInt(p++, term->Id);
                stmt->setInt(p++, classItem->Id);

                if (streamItem != nullptr && streamItem->Id > 0)
                    stmt->setInt(p++, streamItem->Id);

                stmt->setString(
                    p++,
                    msclr::interop::marshal_as<std::string>(feeName)
                );
                stmt->setDouble(p++, Convert::ToDouble(amount));
                stmt->setString(
                    p++,
                    msclr::interop::marshal_as<std::string>(
                        feeStructureDueDatePicker->Value.ToString(L"yyyy-MM-dd")
                    )
                );
                stmt->setString(
                    p++,
                    msclr::interop::marshal_as<std::string>(
                        feeStructureDescriptionBox->Text->Trim()
                    )
                );

                stmt->execute();

                feeStructureEditorForm->DialogResult =
                    System::Windows::Forms::DialogResult::OK;
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    feeStructureEditorForm,
                    gcnew String(ex.what()),
                    L"Unable to Save Fee Structure",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }

        void ShowAddFeeStructureDialog()
        {
            feeStructureEditorForm = gcnew Form();
            feeStructureEditorForm->Text = L"SchoolCore - Add Fee Structure";
            feeStructureEditorForm->StartPosition = FormStartPosition::CenterParent;
            feeStructureEditorForm->FormBorderStyle =
                System::Windows::Forms::FormBorderStyle::FixedDialog;
            feeStructureEditorForm->MaximizeBox = false;
            feeStructureEditorForm->MinimizeBox = false;
            feeStructureEditorForm->ShowInTaskbar = false;
            feeStructureEditorForm->ClientSize = Drawing::Size(640, 470);
            feeStructureEditorForm->BackColor = ThemeManager::Canvas();

            TableLayoutPanel^ root = gcnew TableLayoutPanel();
            root->Dock = DockStyle::Fill;
            root->Padding = System::Windows::Forms::Padding(20);
            root->ColumnCount = 2;
            root->RowCount = 8;
            root->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Absolute, 150.0F));
            root->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Percent, 100.0F));

            for (int i = 0; i < 8; ++i)
                root->RowStyles->Add(gcnew RowStyle(SizeType::Absolute, i == 6 ? 70.0F : 42.0F));

            feeStructureYearBox = gcnew ComboBox();
            feeStructureTermBox = gcnew ComboBox();
            feeStructureClassBox = gcnew ComboBox();
            feeStructureStreamBox = gcnew ComboBox();

            feeStructureYearBox->Dock = DockStyle::Fill;
            feeStructureTermBox->Dock = DockStyle::Fill;
            feeStructureClassBox->Dock = DockStyle::Fill;
            feeStructureStreamBox->Dock = DockStyle::Fill;

            feeStructureNameBox = gcnew TextBox();
            feeStructureAmountBox = gcnew TextBox();
            feeStructureDescriptionBox = gcnew TextBox();

            feeStructureNameBox->Dock = DockStyle::Fill;
            feeStructureAmountBox->Dock = DockStyle::Fill;
            feeStructureDescriptionBox->Dock = DockStyle::Fill;
            feeStructureDescriptionBox->Multiline = true;

            feeStructureDueDatePicker = gcnew DateTimePicker();
            feeStructureDueDatePicker->Dock = DockStyle::Fill;
            feeStructureDueDatePicker->Format = DateTimePickerFormat::Short;
            feeStructureDueDatePicker->Value = DateTime::Today.AddDays(30);

            array<String^>^ labels = gcnew array<String^>
            {
                L"Academic Year",
                L"Term",
                L"Class",
                L"Stream",
                L"Fee Name",
                L"Amount (UGX)",
                L"Due Date",
                L"Description"
            };

            array<Control^>^ inputs = gcnew array<Control^>
            {
                feeStructureYearBox,
                feeStructureTermBox,
                feeStructureClassBox,
                feeStructureStreamBox,
                feeStructureNameBox,
                feeStructureAmountBox,
                feeStructureDueDatePicker,
                feeStructureDescriptionBox
            };

            for (int i = 0; i < labels->Length; ++i)
            {
                root->Controls->Add(CreateFieldLabel(labels[i]), 0, i);
                root->Controls->Add(inputs[i], 1, i);
            }

            FlowLayoutPanel^ footer = gcnew FlowLayoutPanel();
            footer->Dock = DockStyle::Bottom;
            footer->Height = 46;
            footer->FlowDirection = FlowDirection::RightToLeft;
            footer->WrapContents = false;

            Button^ cancel = gcnew Button();
            cancel->Text = L"Cancel";
            cancel->Width = 100;
            cancel->DialogResult =
                System::Windows::Forms::DialogResult::Cancel;

            Button^ save = gcnew Button();
            save->Text = L"Save Structure";
            save->Width = 130;
            save->Click += gcnew EventHandler(
                this,
                &Finance::SaveFeeStructure
            );

            footer->Controls->Add(cancel);
            footer->Controls->Add(save);

            feeStructureYearBox->SelectedIndexChanged +=
                gcnew EventHandler(
                    this,
                    &Finance::FeeStructureYearChanged
                );

            feeStructureClassBox->SelectedIndexChanged +=
                gcnew EventHandler(
                    this,
                    &Finance::FeeStructureClassChanged
                );

            feeStructureEditorForm->Controls->Add(root);
            feeStructureEditorForm->Controls->Add(footer);

            feeStructureEditorForm->AcceptButton = save;
            feeStructureEditorForm->CancelButton = cancel;

            ThemeManager::ApplyToForm(feeStructureEditorForm);

            LoadFeeStructureYears();
            LoadFeeStructureClasses();

            feeStructureEditorForm->ShowDialog(this);

            delete feeStructureEditorForm;
            feeStructureEditorForm = nullptr;
        }

        void LoadFeeStructures()
        {
            if (feeStructuresGrid == nullptr)
                return;

            feeStructuresGrid->Rows->Clear();

            try
            {
                auto con = DbConnection::GetConnection();
                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT fs.fee_structure_id, ay.year_name, t.term_name, "
                        "c.class_name, COALESCE(st.stream_name, 'All Streams') AS stream_name, "
                        "fs.fee_name, fs.amount, fs.due_date, fs.status "
                        "FROM fee_structures fs "
                        "INNER JOIN academic_years ay ON ay.academic_year_id = fs.academic_year_id "
                        "INNER JOIN terms t ON t.term_id = fs.term_id "
                        "INNER JOIN classes c ON c.class_id = fs.class_id "
                        "LEFT JOIN streams st ON st.stream_id = fs.stream_id "
                        "ORDER BY ay.start_date DESC, t.term_id, c.class_name, fs.fee_name"
                    )
                );

                std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());
                int count = 0;

                while (result->next())
                {
                    Decimal amount =
                        Decimal::Parse(
                            gcnew String(result->getString("amount").c_str()),
                            System::Globalization::CultureInfo::InvariantCulture
                        );

                    feeStructuresGrid->Rows->Add(
                        result->getInt("fee_structure_id"),
                        gcnew String(result->getString("year_name").c_str()),
                        gcnew String(result->getString("term_name").c_str()),
                        gcnew String(result->getString("class_name").c_str()),
                        gcnew String(result->getString("stream_name").c_str()),
                        gcnew String(result->getString("fee_name").c_str()),
                        amount.ToString(L"N2", System::Globalization::CultureInfo::InvariantCulture),
                        gcnew String(result->getString("due_date").c_str()),
                        gcnew String(result->getString("status").c_str())
                    );
                    ++count;
                }

                feeStructuresSummaryLabel->Text =
                    count.ToString() + L" fee structure(s)";
            }
            catch (sql::SQLException& ex)
            {
                feeStructuresSummaryLabel->Text = L"Unable to load fee structures.";
                MessageBox::Show(
                    feeStructuresForm,
                    gcnew String(ex.what()),
                    L"Fee Structures",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }

        void DeactivateFeeStructure(Object^ sender, EventArgs^ e)
        {
            if (feeStructuresGrid == nullptr ||
                feeStructuresGrid->SelectedRows->Count == 0)
                return;

            int structureId =
                Convert::ToInt32(
                    feeStructuresGrid->SelectedRows[0]->Cells[0]->Value
                );

            if (MessageBox::Show(
                    feeStructuresForm,
                    L"Deactivate the selected fee structure?",
                    L"Fee Structures",
                    MessageBoxButtons::YesNo,
                    MessageBoxIcon::Question
                ) != System::Windows::Forms::DialogResult::Yes)
                return;

            try
            {
                auto con = DbConnection::GetConnection();
                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "UPDATE fee_structures "
                        "SET status = 'Inactive' "
                        "WHERE fee_structure_id = ?"
                    )
                );
                stmt->setInt(1, structureId);
                stmt->execute();

                LoadFeeStructures();
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    feeStructuresForm,
                    gcnew String(ex.what()),
                    L"Unable to Deactivate Fee Structure",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }

        void AddFeeStructureClick(Object^ sender, EventArgs^ e)
        {
            ShowAddFeeStructureDialog();
            LoadFeeStructures();
        }

        void OpenFeeStructuresDialog()
        {
            feeStructuresForm = gcnew Form();
            feeStructuresForm->Text = L"SchoolCore - Fee Structures";
            feeStructuresForm->StartPosition = FormStartPosition::CenterParent;
            feeStructuresForm->ClientSize = Drawing::Size(1000, 620);
            feeStructuresForm->MinimumSize = Drawing::Size(900, 560);
            feeStructuresForm->BackColor = ThemeManager::Canvas();
            feeStructuresForm->ShowInTaskbar = false;

            TableLayoutPanel^ root = gcnew TableLayoutPanel();
            root->Dock = DockStyle::Fill;
            root->Padding = System::Windows::Forms::Padding(20);
            root->ColumnCount = 1;
            root->RowCount = 4;
            root->RowStyles->Add(gcnew RowStyle(SizeType::Absolute, 46.0F));
            root->RowStyles->Add(gcnew RowStyle(SizeType::Absolute, 48.0F));
            root->RowStyles->Add(gcnew RowStyle(SizeType::Percent, 100.0F));
            root->RowStyles->Add(gcnew RowStyle(SizeType::Absolute, 54.0F));

            Label^ title = CreateLabel(
                L"Fee Structures",
                FinanceTheme::Dialog,
                FinanceTheme::TextStrong
            );
            title->Dock = DockStyle::Fill;
            title->TextAlign = ContentAlignment::MiddleLeft;

            FlowLayoutPanel^ toolbar = gcnew FlowLayoutPanel();
            toolbar->Dock = DockStyle::Fill;
            toolbar->WrapContents = false;
            toolbar->FlowDirection = FlowDirection::LeftToRight;

            Button^ add = gcnew Button();
            add->Text = L"Add Fee Structure";
            add->Width = 145;
            add->Click += gcnew EventHandler(
                this,
                &Finance::AddFeeStructureClick
            );

            Button^ deactivate = gcnew Button();
            deactivate->Text = L"Deactivate";
            deactivate->Width = 105;
            deactivate->Click += gcnew EventHandler(
                this,
                &Finance::DeactivateFeeStructure
            );

            toolbar->Controls->Add(add);
            toolbar->Controls->Add(deactivate);

            feeStructuresSummaryLabel =
                CreateLabel(
                    L"0 fee structure(s)",
                    FinanceTheme::Small,
                    FinanceTheme::TextMuted
                );
            feeStructuresSummaryLabel->Dock = DockStyle::Fill;

            feeStructuresGrid = gcnew DataGridView();
            feeStructuresGrid->Dock = DockStyle::Fill;
            feeStructuresGrid->ReadOnly = true;
            feeStructuresGrid->AllowUserToAddRows = false;
            feeStructuresGrid->AllowUserToDeleteRows = false;
            feeStructuresGrid->MultiSelect = false;
            feeStructuresGrid->SelectionMode = DataGridViewSelectionMode::FullRowSelect;
            feeStructuresGrid->AutoGenerateColumns = false;
            feeStructuresGrid->RowHeadersVisible = false;

            array<String^>^ headers = gcnew array<String^>
            {
                L"ID", L"Academic Year", L"Term", L"Class", L"Stream",
                L"Fee", L"Amount (UGX)", L"Due Date", L"Status"
            };

            array<int>^ widths = gcnew array<int>
            {
                55, 105, 90, 90, 100, 160, 115, 100, 85
            };

            for (int i = 0; i < headers->Length; ++i)
            {
                DataGridViewTextBoxColumn^ col =
                    gcnew DataGridViewTextBoxColumn();
                col->HeaderText = headers[i];
                col->Name = L"Col" + i.ToString();
                col->Width = widths[i];
                feeStructuresGrid->Columns->Add(col);
            }

            Button^ close = gcnew Button();
            close->Text = L"Close";
            close->Width = 100;
            close->DialogResult =
                System::Windows::Forms::DialogResult::Cancel;

            root->Controls->Add(title, 0, 0);
            root->Controls->Add(toolbar, 0, 1);
            root->Controls->Add(feeStructuresGrid, 0, 2);
            root->Controls->Add(feeStructuresSummaryLabel, 0, 3);

            feeStructuresForm->Controls->Add(root);
            feeStructuresForm->Controls->Add(close);

            close->Anchor = AnchorStyles::Top | AnchorStyles::Right;
            close->Location = Drawing::Point(870, 12);

            feeStructuresForm->CancelButton = close;

            ThemeManager::ApplyToForm(feeStructuresForm);
            LoadFeeStructures();

            feeStructuresForm->ShowDialog(this);

            delete feeStructuresForm;
            feeStructuresForm = nullptr;
            feeStructuresGrid = nullptr;
            feeStructuresSummaryLabel = nullptr;
        }

        void LoadStudentChargeStructures()
        {
            studentChargesStructureBox->Items->Clear();
            studentChargesStructureBox->Items->Add(
                gcnew FinanceFeeStructureItem(
                    0, L"Select fee structure", L"", L"", L"", L"",
                    Decimal(0), L""
                )
            );

            try
            {
                auto con = DbConnection::GetConnection();
                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT fs.fee_structure_id, fs.fee_name, ay.year_name, "
                        "t.term_name, c.class_name, COALESCE(st.stream_name, 'All Streams') AS stream_name, "
                        "fs.amount, fs.due_date "
                        "FROM fee_structures fs "
                        "INNER JOIN academic_years ay ON ay.academic_year_id = fs.academic_year_id "
                        "INNER JOIN terms t ON t.term_id = fs.term_id "
                        "INNER JOIN classes c ON c.class_id = fs.class_id "
                        "LEFT JOIN streams st ON st.stream_id = fs.stream_id "
                        "WHERE fs.status = 'Active' "
                        "ORDER BY ay.start_date DESC, t.term_id, c.class_name, fs.fee_name"
                    )
                );

                std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());

                while (result->next())
                {
                    Decimal amount =
                        Decimal::Parse(
                            gcnew String(result->getString("amount").c_str()),
                            System::Globalization::CultureInfo::InvariantCulture
                        );

                    studentChargesStructureBox->Items->Add(
                        gcnew FinanceFeeStructureItem(
                            result->getInt("fee_structure_id"),
                            gcnew String(result->getString("fee_name").c_str()),
                            gcnew String(result->getString("year_name").c_str()),
                            gcnew String(result->getString("term_name").c_str()),
                            gcnew String(result->getString("class_name").c_str()),
                            gcnew String(result->getString("stream_name").c_str()),
                            amount,
                            gcnew String(result->getString("due_date").c_str())
                        )
                    );
                }

                studentChargesStructureBox->SelectedIndex = 0;
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    studentChargesForm,
                    gcnew String(ex.what()),
                    L"Unable to Load Fee Structures",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }

        void LoadStudentCharges()
        {
            if (studentChargesGrid == nullptr)
                return;

            studentChargesGrid->Rows->Clear();

            try
            {
                auto con = DbConnection::GetConnection();
                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT fc.fee_charge_id, s.registration_number, "
                        "CONCAT_WS(' ', s.first_name, s.middle_name, s.last_name) AS student_name, "
                        "fs.fee_name, ay.year_name, t.term_name, c.class_name, "
                        "COALESCE(st.stream_name, 'All Streams') AS stream_name, "
                        "fc.amount, fc.due_date, fc.status, "
                        "COALESCE(SUM(pa.amount), 0) AS paid, "
                        "fc.amount - COALESCE(SUM(pa.amount), 0) AS balance "
                        "FROM fee_charges fc "
                        "INNER JOIN students s ON s.student_id = fc.student_id "
                        "INNER JOIN fee_structures fs ON fs.fee_structure_id = fc.fee_structure_id "
                        "INNER JOIN academic_years ay ON ay.academic_year_id = fs.academic_year_id "
                        "INNER JOIN terms t ON t.term_id = fs.term_id "
                        "INNER JOIN classes c ON c.class_id = fs.class_id "
                        "LEFT JOIN streams st ON st.stream_id = fs.stream_id "
                        "LEFT JOIN payment_allocations pa ON pa.fee_charge_id = fc.fee_charge_id "
                        "GROUP BY fc.fee_charge_id, s.registration_number, s.first_name, s.middle_name, "
                        "s.last_name, fs.fee_name, ay.year_name, t.term_name, c.class_name, "
                        "st.stream_name, fc.amount, fc.due_date, fc.status "
                        "ORDER BY fc.fee_charge_id DESC"
                    )
                );

                std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());
                Decimal total = Decimal(0);
                Decimal outstanding = Decimal(0);
                int count = 0;

                while (result->next())
                {
                    Decimal amount = Decimal::Parse(
                        gcnew String(result->getString("amount").c_str()),
                        System::Globalization::CultureInfo::InvariantCulture
                    );
                    Decimal balance = Decimal::Parse(
                        gcnew String(result->getString("balance").c_str()),
                        System::Globalization::CultureInfo::InvariantCulture
                    );

                    studentChargesGrid->Rows->Add(
                        result->getInt("fee_charge_id"),
                        gcnew String(result->getString("registration_number").c_str()),
                        gcnew String(result->getString("student_name").c_str()),
                        gcnew String(result->getString("fee_name").c_str()),
                        gcnew String(result->getString("year_name").c_str()),
                        gcnew String(result->getString("term_name").c_str()),
                        gcnew String(result->getString("class_name").c_str()),
                        gcnew String(result->getString("stream_name").c_str()),
                        amount.ToString(L"N2", System::Globalization::CultureInfo::InvariantCulture),
                        gcnew String(result->getString("due_date").c_str()),
                        gcnew String(result->getString("status").c_str()),
                        balance.ToString(L"N2", System::Globalization::CultureInfo::InvariantCulture)
                    );

                    total += amount;
                    outstanding += balance;
                    ++count;
                }

                studentChargesSummaryLabel->Text =
                    count.ToString() +
                    L" charge(s)  |  Total: UGX " +
                    total.ToString(L"N2", System::Globalization::CultureInfo::InvariantCulture) +
                    L"  |  Outstanding: UGX " +
                    outstanding.ToString(L"N2", Globalization::CultureInfo.InvariantCulture);
            }
            catch (sql::SQLException& ex)
            {
                studentChargesSummaryLabel->Text =
                    L"Unable to load student charges.";
                MessageBox::Show(
                    studentChargesForm,
                    gcnew String(ex.what()),
                    L"Student Charges",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }

        void GenerateStudentCharges(Object^ sender, EventArgs^ e)
        {
            FinanceFeeStructureItem^ structure =
                dynamic_cast<FinanceFeeStructureItem^>(
                    studentChargesStructureBox->SelectedItem
                );

            if (structure == nullptr || structure->Id <= 0)
            {
                MessageBox::Show(
                    studentChargesForm,
                    L"First create and select a fee structure.",
                    L"Student Charges",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                return;
            }

            std::unique_ptr<sql::Connection> conOwner;
            sql::Connection* con = nullptr;
            bool transactionStarted = false;

            try
            {
                conOwner = DbConnection::GetConnection();
                con = conOwner.get();
                con->setAutoCommit(false);
                transactionStarted = true;

                std::unique_ptr<sql::PreparedStatement> getStructure(
                    con->prepareStatement(
                        "SELECT academic_year_id, term_id, class_id, stream_id, amount, due_date "
                        "FROM fee_structures "
                        "WHERE fee_structure_id = ? AND status = 'Active' "
                        "FOR UPDATE"
                    )
                );
                getStructure->setInt(1, structure->Id);

                std::unique_ptr<sql::ResultSet> sr(getStructure->executeQuery());

                if (!sr->next())
                    throw gcnew Exception(L"The selected fee structure is not active or no longer exists.");

                int academicYearId = sr->getInt("academic_year_id");
                int termId = sr->getInt("term_id");
                int classId = sr->getInt("class_id");
                int streamId = sr->getInt("stream_id");

                Decimal amount = Decimal::Parse(
                    gcnew String(sr->getString("amount").c_str()),
                    System::Globalization::CultureInfo::InvariantCulture
                );

                String^ dueDate =
                    gcnew String(sr->getString("due_date").c_str());

                String^ sqlText;

                if (sr->isNull("stream_id"))
                {
                    sqlText =
                        L"INSERT INTO fee_charges "
                        L"(student_id, fee_structure_id, amount, due_date, status) "
                        L"SELECT e.student_id, ?, ?, ?, 'Pending' "
                        L"FROM enrollments e "
                        L"WHERE e.academic_year_id = ? "
                        L"AND e.term_id = ? "
                        L"AND e.class_id = ? "
                        L"AND e.status = 'Active' "
                        L"AND NOT EXISTS ("
                        L"    SELECT 1 FROM fee_charges fc "
                        L"    WHERE fc.student_id = e.student_id "
                        L"    AND fc.fee_structure_id = ?"
                        L")";
                }
                else
                {
                    sqlText =
                        L"INSERT INTO fee_charges "
                        L"(student_id, fee_structure_id, amount, due_date, status) "
                        L"SELECT e.student_id, ?, ?, ?, 'Pending' "
                        L"FROM enrollments e "
                        L"WHERE e.academic_year_id = ? "
                        L"AND e.term_id = ? "
                        L"AND e.class_id = ? "
                        L"AND e.stream_id = ? "
                        L"AND e.status = 'Active' "
                        L"AND NOT EXISTS ("
                        L"    SELECT 1 FROM fee_charges fc "
                        L"    WHERE fc.student_id = e.student_id "
                        L"    AND fc.fee_structure_id = ?"
                        L")";
                }

                std::unique_ptr<sql::PreparedStatement> insertCharges(
                    con->prepareStatement(
                        msclr::interop::marshal_as<std::string>(sqlText)
                    )
                );

                int p = 1;
                insertCharges->setInt(p++, structure->Id);
                insertCharges->setDouble(p++, Convert::ToDouble(amount));
                insertCharges->setString(
                    p++,
                    msclr::interop::marshal_as<std::string>(dueDate)
                );
                insertCharges->setInt(p++, academicYearId);
                insertCharges->setInt(p++, termId);
                insertCharges->setInt(p++, classId);

                if (!sr->isNull("stream_id"))
                    insertCharges->setInt(p++, streamId);

                insertCharges->setInt(p++, structure->Id);

                int created = insertCharges->executeUpdate();

                con->commit();
                con->setAutoCommit(true);
                transactionStarted = false;

                LoadStudentCharges();

                MessageBox::Show(
                    studentChargesForm,
                    created.ToString() +
                    L" student charge(s) generated from the selected fee structure.",
                    L"Student Charges",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information
                );
            }
            catch (sql::SQLException& ex)
            {
                if (transactionStarted && con != nullptr)
                {
                    try
                    {
                        con->rollback();
                        con->setAutoCommit(true);
                    }
                    catch (Exception^)
                    {
                    }
                }

                MessageBox::Show(
                    studentChargesForm,
                    gcnew String(ex.what()),
                    L"Unable to Generate Student Charges",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
            catch (Exception^ ex)
            {
                if (transactionStarted && con != nullptr)
                {
                    try
                    {
                        con->rollback();
                        con->setAutoCommit(true);
                    }
                    catch (Exception^)
                    {
                    }
                }

                MessageBox::Show(
                    studentChargesForm,
                    ex->Message,
                    L"Unable to Generate Student Charges",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }

        void OpenStudentChargesDialog()
        {
            studentChargesForm = gcnew Form();
            studentChargesForm->Text = L"SchoolCore - Student Charges";
            studentChargesForm->StartPosition = FormStartPosition::CenterParent;
            studentChargesForm->ClientSize = Drawing::Size(1200, 650);
            studentChargesForm->MinimumSize = Drawing::Size(1000, 560);
            studentChargesForm->BackColor = ThemeManager::Canvas();
            studentChargesForm->ShowInTaskbar = false;

            TableLayoutPanel^ root = gcnew TableLayoutPanel();
            root->Dock = DockStyle::Fill;
            root->Padding = System::Windows::Forms::Padding(20);
            root->ColumnCount = 1;
            root->RowCount = 4;
            root->RowStyles->Add(gcnew RowStyle(SizeType::Absolute, 46.0F));
            root->RowStyles->Add(gcnew RowStyle(SizeType::Absolute, 54.0F));
            root->RowStyles->Add(gcnew RowStyle(SizeType::Percent, 100.0F));
            root->RowStyles->Add(gcnew RowStyle(SizeType::Absolute, 42.0F));

            Label^ title = CreateLabel(
                L"Student Charges",
                FinanceTheme::Dialog,
                FinanceTheme::TextStrong
            );
            title->Dock = DockStyle::Fill;
            title->TextAlign = ContentAlignment::MiddleLeft;

            TableLayoutPanel^ toolbar = gcnew TableLayoutPanel();
            toolbar->Dock = DockStyle::Fill;
            toolbar->ColumnCount = 3;
            toolbar->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Absolute, 130.0F));
            toolbar->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Percent, 100.0F));
            toolbar->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Absolute, 145.0F));

            toolbar->Controls->Add(
                CreateFieldLabel(L"Fee Structure"),
                0, 0
            );

            studentChargesStructureBox = gcnew ComboBox();
            studentChargesStructureBox->Dock = DockStyle::Fill;
            studentChargesStructureBox->DropDownStyle =
                ComboBoxStyle::DropDownList;

            toolbar->Controls->Add(
                studentChargesStructureBox,
                1, 0
            );

            Button^ generate = gcnew Button();
            generate->Text = L"Generate Charges";
            generate->Dock = DockStyle::Fill;
            generate->Click += gcnew EventHandler(
                this,
                &Finance::GenerateStudentCharges
            );

            toolbar->Controls->Add(generate, 2, 0);

            studentChargesGrid = gcnew DataGridView();
            studentChargesGrid->Dock = DockStyle::Fill;
            studentChargesGrid->ReadOnly = true;
            studentChargesGrid->AllowUserToAddRows = false;
            studentChargesGrid->AllowUserToDeleteRows = false;
            studentChargesGrid->MultiSelect = false;
            studentChargesGrid->SelectionMode =
                DataGridViewSelectionMode::FullRowSelect;
            studentChargesGrid->AutoGenerateColumns = false;
            studentChargesGrid->RowHeadersVisible = false;

            array<String^>^ headers = gcnew array<String^>
            {
                L"ID", L"Registration", L"Student", L"Fee",
                L"Year", L"Term", L"Class", L"Stream",
                L"Amount (UGX)", L"Due Date", L"Status", L"Balance (UGX)"
            };

            array<int>^ widths = gcnew array<int>
            {
                50, 110, 170, 150, 90, 80, 85, 95, 110, 95, 95, 120
            };

            for (int i = 0; i < headers->Length; ++i)
            {
                DataGridViewTextBoxColumn^ col =
                    gcnew DataGridViewTextBoxColumn();
                col->HeaderText = headers[i];
                col->Name = L"Col" + i.ToString();
                col->Width = widths[i];
                studentChargesGrid->Columns->Add(col);
            }

            studentChargesSummaryLabel =
                CreateLabel(
                    L"0 charge(s)",
                    FinanceTheme::Small,
                    FinanceTheme::TextMuted
                );
            studentChargesSummaryLabel->Dock = DockStyle::Fill;

            root->Controls->Add(title, 0, 0);
            root->Controls->Add(toolbar, 0, 1);
            root->Controls->Add(studentChargesGrid, 0, 2);
            root->Controls->Add(studentChargesSummaryLabel, 0, 3);

            studentChargesForm->Controls->Add(root);

            ThemeManager::ApplyToForm(studentChargesForm);

            LoadStudentChargeStructures();
            LoadStudentCharges();

            studentChargesForm->ShowDialog(this);

            delete studentChargesForm;
            studentChargesForm = nullptr;
            studentChargesGrid = nullptr;
            studentChargesStructureBox = nullptr;
            studentChargesSummaryLabel = nullptr;
        }

        void SetMetric(int index, Decimal amount)
        {
            if (index < 0 || index >= MetricCount)
                return;

            metricValues[index]->Text =
                L"UGX " + amount.ToString(
                    L"N2",
                    System::Globalization::CultureInfo::InvariantCulture);
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

            if (operation->Title->Equals(L"Fee Structures"))
            {
                OpenFeeStructuresDialog();
                return;
            }

            if (operation->Title->Equals(L"Student Charges"))
            {
                OpenStudentChargesDialog();
                return;
            }

            if (operation->Title->Equals(L"Record Payment"))
            {
                OpenRecordPaymentDialog();
                return;
            }

            if (operation->Title->Equals(L"Payment History"))
            {
                OpenPaymentHistoryDialog();
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
                    L"Generate student charges from fee structures for active enrollments and review balances."),
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

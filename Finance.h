#pragma once

#include "AuthSession.h"

namespace SchoolCore
{
    using namespace System;
    using namespace System::Drawing;
    using namespace System::Windows::Forms;

    public ref class Finance : public Form
    {
    public:
        Finance()
        {
            InitializeComponent();
        }

    private:
        Button^ btnFeeStructures;
        Button^ btnStudentCharges;
        Button^ btnRecordPayment;
        Button^ btnPaymentHistory;

        void OpenFinanceOperation(String^ titleText, String^ descriptionText)
        {
            Form^ view = gcnew Form();
            view->Text = L"SchoolCore - " + titleText;
            view->StartPosition = FormStartPosition::CenterParent;
            view->ClientSize = Drawing::Size(900, 560);
            view->MinimumSize = Drawing::Size(800, 500);
            view->BackColor = Color::White;
            view->ShowInTaskbar = false;
            view->MinimizeBox = false;
            view->MaximizeBox = false;

            Panel^ header = gcnew Panel();
            header->Dock = DockStyle::Top;
            header->Height = 96;
            header->BackColor = Color::FromArgb(248, 250, 252);
            header->Padding = System::Windows::Forms::Padding(28, 18, 28, 12);

            Label^ heading = gcnew Label();
            heading->Text = titleText;
            heading->UseMnemonic = false;
            heading->Dock = DockStyle::Top;
            heading->Height = 36;
            heading->Font = gcnew Drawing::Font(L"Segoe UI Semibold", 18.0F, FontStyle::Bold);
            heading->ForeColor = Color::FromArgb(15, 23, 42);

            Label^ description = gcnew Label();
            description->Text = descriptionText;
            description->Dock = DockStyle::Fill;
            description->Font = gcnew Drawing::Font(L"Segoe UI", 10.0F);
            description->ForeColor = Color::FromArgb(71, 85, 105);

            Button^ close = gcnew Button();
            close->Text = L"Close";
            close->DialogResult = DialogResult::Cancel;
            close->Anchor = AnchorStyles::Top | AnchorStyles::Right;
            close->Size = Drawing::Size(100, 32);
            close->Location = Drawing::Point(772, 510);

            header->Controls->Add(description);
            header->Controls->Add(heading);
            view->Controls->Add(close);
            view->Controls->Add(header);
            view->CancelButton = close;
            view->AcceptButton = nullptr;
            view->ShowDialog(this);
            delete view;
        }

        void btnFeeStructures_Click(Object^ sender, EventArgs^ e)
        {
            OpenFinanceOperation(L"Fee Structures", L"Define and manage school fee structures by academic year, term, class and stream.");
        }

        void btnStudentCharges_Click(Object^ sender, EventArgs^ e)
        {
            OpenFinanceOperation(L"Student Charges", L"Review student fee charges, due dates, payment status and outstanding balances.");
        }

        void btnRecordPayment_Click(Object^ sender, EventArgs^ e)
        {
            OpenFinanceOperation(L"Record Payment", L"Record cash, Mobile Money, bank, card or other supported payments with a transaction reference.");
        }

        void btnPaymentHistory_Click(Object^ sender, EventArgs^ e)
        {
            OpenFinanceOperation(L"Payment History", L"Review recorded payments, receipts, payment methods and transaction references.");
        }

        void InitializeComponent()
        {
            this->SuspendLayout();

            this->Text = L"SchoolCore - Fees & Finance";
            this->StartPosition = FormStartPosition::CenterScreen;
            this->ClientSize = Drawing::Size(1000, 650);
            this->MinimumSize = Drawing::Size(900, 600);
            this->BackColor = Color::White;
            this->DoubleBuffered = true;

            Panel^ header = gcnew Panel();
            header->Dock = DockStyle::Top;
            header->Height = 105;
            header->BackColor = Color::FromArgb(248, 250, 252);
            header->Padding = System::Windows::Forms::Padding(28, 18, 28, 12);

            Label^ title = gcnew Label();
            title->Text = L"Fees & Finance";
            title->Dock = DockStyle::Top;
            title->Height = 42;
            title->UseMnemonic = false;
            title->Font = gcnew Drawing::Font(L"Segoe UI Semibold", 21.0F, FontStyle::Bold);
            title->ForeColor = Color::FromArgb(15, 23, 42);

            Label^ subtitle = gcnew Label();
            subtitle->Text = L"Manage fee structures, student charges, payments and financial records.";
            subtitle->Dock = DockStyle::Fill;
            subtitle->Font = gcnew Drawing::Font(L"Segoe UI", 10.0F);
            subtitle->ForeColor = Color::FromArgb(71, 85, 105);

            header->Controls->Add(subtitle);
            header->Controls->Add(title);

            Panel^ content = gcnew Panel();
            content->Dock = DockStyle::Fill;
            content->Padding = System::Windows::Forms::Padding(28);
            content->BackColor = Color::White;

            Label^ overview = gcnew Label();
            overview->Text = L"Financial Overview";
            overview->Dock = DockStyle::Top;
            overview->Height = 42;
            overview->Font = gcnew Drawing::Font(L"Segoe UI Semibold", 13.0F, FontStyle::Bold);
            overview->ForeColor = Color::FromArgb(30, 41, 59);

            TableLayoutPanel^ metrics = gcnew TableLayoutPanel();
            metrics->Dock = DockStyle::Top;
            metrics->Height = 135;
            metrics->ColumnCount = 4;
            metrics->RowCount = 1;
            for (int i = 0; i < 4; ++i)
                metrics->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Percent, 25.0F));

            array<String^>^ metricNames = gcnew array<String^>
            {
                L"Total Revenue", L"Outstanding Fees", L"Collected Today", L"Pending Approvals"
            };

            for (int i = 0; i < 4; ++i)
            {
                Panel^ card = gcnew Panel();
                card->Dock = DockStyle::Fill;
                card->Margin = System::Windows::Forms::Padding(0, 0, 12, 10);
                card->Padding = System::Windows::Forms::Padding(15, 12, 15, 12);
                card->BackColor = Color::FromArgb(248, 250, 252);
                card->BorderStyle = BorderStyle::FixedSingle;

                Label^ name = gcnew Label();
                name->Text = metricNames[i];
                name->Dock = DockStyle::Top;
                name->Height = 28;
                name->Font = gcnew Drawing::Font(L"Segoe UI", 9.0F);
                name->ForeColor = Color::FromArgb(71, 85, 105);

                Label^ value = gcnew Label();
                value->Text = L"UGX 0.00";
                value->TextAlign = ContentAlignment::MiddleLeft;
                value->Dock = DockStyle::Fill;
                value->Font = gcnew Drawing::Font(L"Segoe UI Semibold", 18.0F, FontStyle::Bold);
                value->ForeColor = Color::FromArgb(15, 23, 42);

                card->Controls->Add(value);
                card->Controls->Add(name);
                metrics->Controls->Add(card, i, 0);
            }

            Label^ operations = gcnew Label();
            operations->Text = L"Finance Operations";
            operations->Dock = DockStyle::Top;
            operations->Height = 45;
            operations->Padding = System::Windows::Forms::Padding(0, 8, 0, 0);
            operations->Font = gcnew Drawing::Font(L"Segoe UI Semibold", 13.0F, FontStyle::Bold);
            operations->ForeColor = Color::FromArgb(30, 41, 59);

            TableLayoutPanel^ actions = gcnew TableLayoutPanel();
            actions->Dock = DockStyle::Top;
            actions->Height = 190;
            actions->ColumnCount = 2;
            actions->RowCount = 2;
            actions->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Percent, 50.0F));
            actions->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Percent, 50.0F));
            actions->RowStyles->Add(gcnew RowStyle(SizeType::Percent, 50.0F));
            actions->RowStyles->Add(gcnew RowStyle(SizeType::Percent, 50.0F));

            this->btnFeeStructures = gcnew Button();
            this->btnStudentCharges = gcnew Button();
            this->btnRecordPayment = gcnew Button();
            this->btnPaymentHistory = gcnew Button();

            array<Button^>^ buttons = gcnew array<Button^>
            {
                btnFeeStructures, btnStudentCharges, btnRecordPayment, btnPaymentHistory
            };
            array<String^>^ texts = gcnew array<String^>
            {
                L"  Fee Structures", L"  Student Charges",
                L"  Record Payment", L"  Payment History"
            };

            for (int i = 0; i < 4; ++i)
            {
                buttons[i]->Text = texts[i];
                buttons[i]->Dock = DockStyle::Fill;
                buttons[i]->Margin = System::Windows::Forms::Padding(0, 0, 12, 12);
                buttons[i]->FlatStyle = FlatStyle::Flat;
                buttons[i]->FlatAppearance->BorderSize = 1;
                buttons[i]->BackColor = Color::White;
                buttons[i]->ForeColor = Color::FromArgb(15, 23, 42);
                buttons[i]->Font = gcnew Drawing::Font(L"Segoe UI Semibold", 10.0F);
                buttons[i]->TextAlign = ContentAlignment::MiddleLeft;
                buttons[i]->Padding = System::Windows::Forms::Padding(18, 0, 10, 0);
                buttons[i]->FlatAppearance->MouseOverBackColor = Color::FromArgb(241, 245, 249);
                buttons[i]->FlatAppearance->MouseDownBackColor = Color::FromArgb(226, 232, 240);
                actions->Controls->Add(buttons[i], i % 2, i / 2);
            }

            btnFeeStructures->Click += gcnew EventHandler(this, &Finance::btnFeeStructures_Click);
            btnStudentCharges->Click += gcnew EventHandler(this, &Finance::btnStudentCharges_Click);
            btnRecordPayment->Click += gcnew EventHandler(this, &Finance::btnRecordPayment_Click);
            btnPaymentHistory->Click += gcnew EventHandler(this, &Finance::btnPaymentHistory_Click);

            Label^ note = gcnew Label();
            note->Text = L"Electronic payments use a payment method and transaction reference. Sensitive credentials such as PINs, CVVs and full card numbers are never stored.";
            note->Dock = DockStyle::Fill;
            note->Font = gcnew Drawing::Font(L"Segoe UI", 9.0F);
            note->ForeColor = Color::FromArgb(71, 85, 105);
            note->Padding = System::Windows::Forms::Padding(0, 18, 0, 0);

            content->Controls->Add(note);
            content->Controls->Add(actions);
            content->Controls->Add(operations);
            content->Controls->Add(metrics);
            content->Controls->Add(overview);

            this->Controls->Add(content);
            this->Controls->Add(header);

            this->ResumeLayout(false);
        }
    };
}
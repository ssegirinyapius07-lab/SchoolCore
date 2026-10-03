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

        void InitializeComponent()
        {
            this->Text = L"SchoolCore - Fees & Finance";
            this->StartPosition = FormStartPosition::CenterScreen;
            this->ClientSize = Drawing::Size(1000, 650);
            this->MinimumSize = Drawing::Size(900, 600);
            this->BackColor = Color::White;

            Panel^ header = gcnew Panel();
            header->Dock = DockStyle::Top;
            header->Height = 105;
            header->BackColor = Color::FromArgb(248, 250, 252);
            header->Padding = Padding(28, 18, 28, 12);

            Label^ title = gcnew Label();
            title->Text = L"Fees & Finance";
            title->Dock = DockStyle::Top;
            title->Height = 42;
            title->Font = gcnew Drawing::Font(L"Segoe UI Semibold", 21.0F, FontStyle::Bold);
            title->ForeColor = Color::FromArgb(15, 23, 42);

            Label^ subtitle = gcnew Label();
            subtitle->Text = L"Manage fee structures, student charges, payments and financial records.";
            subtitle->Dock = DockStyle::Fill;
            subtitle->Font = gcnew Drawing::Font(L"Segoe UI", 10.0F);
            subtitle->ForeColor = Color::FromArgb(71, 85, 105);

            header->Controls->Add(subtitle);
            header->Controls->Add(title);
            this->Controls->Add(header);

            Panel^ content = gcnew Panel();
            content->Dock = DockStyle::Fill;
            content->Padding = Padding(28);
            content->BackColor = Color::White;

            Label^ overview = gcnew Label();
            overview->Text = L"Financial Overview";
            overview->Dock = DockStyle::Top;
            overview->Height = 42;
            overview->Font = gcnew Drawing::Font(L"Segoe UI Semibold", 13.0F, FontStyle::Bold);
            overview->ForeColor = Color::FromArgb(30, 41, 59);

            TableLayoutPanel^ metrics = gcnew TableLayoutPanel();
            metrics->Dock = DockStyle::Top;
            metrics->Height = 115;
            metrics->ColumnCount = 4;
            metrics->RowCount = 1;
            for (int i = 0; i < 4; ++i)
                metrics->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Percent, 25.0F));

            array<String^>^ metricNames = gcnew array<String^>
            {
                L"Total Charges", L"Total Paid", L"Outstanding", L"Payments Today"
            };

            for (int i = 0; i < 4; ++i)
            {
                Panel^ card = gcnew Panel();
                card->Dock = DockStyle::Fill;
                card->Margin = Padding(0, 0, 12, 10);
                card->Padding = Padding(15);
                card->BackColor = Color::FromArgb(248, 250, 252);
                card->BorderStyle = BorderStyle::FixedSingle;

                Label^ name = gcnew Label();
                name->Text = metricNames[i];
                name->Dock = DockStyle::Top;
                name->Height = 28;
                name->Font = gcnew Drawing::Font(L"Segoe UI", 9.0F);
                name->ForeColor = Color::FromArgb(71, 85, 105);

                Label^ value = gcnew Label();
                value->Text = L"—";
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
            operations->Padding = Padding(0, 8, 0, 0);
            operations->Font = gcnew Drawing::Font(L"Segoe UI Semibold", 13.0F, FontStyle::Bold);
            operations->ForeColor = Color::FromArgb(30, 41, 59);

            TableLayoutPanel^ actions = gcnew TableLayoutPanel();
            actions->Dock = DockStyle::Top;
            actions->Height = 155;
            actions->ColumnCount = 2;
            actions->RowCount = 2;
            actions->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Percent, 50.0F));
            actions->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Percent, 50.0F));

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
                buttons[i]->Margin = Padding(0, 0, 12, 12);
                buttons[i]->FlatStyle = FlatStyle::Flat;
                buttons[i]->FlatAppearance->BorderSize = 1;
                buttons[i]->BackColor = Color::White;
                buttons[i]->ForeColor = Color::FromArgb(15, 23, 42);
                buttons[i]->Font = gcnew Drawing::Font(L"Segoe UI Semibold", 10.0F);
                buttons[i]->TextAlign = ContentAlignment::MiddleLeft;
                buttons[i]->Padding = Padding(18, 0, 10, 0);
                actions->Controls->Add(buttons[i], i % 2, i / 2);
            }

            Label^ note = gcnew Label();
            note->Text = L"Electronic payments use a payment method and transaction reference. Sensitive credentials such as PINs, CVVs and full card numbers are never stored.";
            note->Dock = DockStyle::Fill;
            note->Font = gcnew Drawing::Font(L"Segoe UI", 9.0F);
            note->ForeColor = Color::FromArgb(71, 85, 105);
            note->Padding = Padding(0, 18, 0, 0);

            content->Controls->Add(note);
            content->Controls->Add(actions);
            content->Controls->Add(operations);
            content->Controls->Add(metrics);
            content->Controls->Add(overview);
            this->Controls->Add(content);

            if (!AuthSession::HasPermission(L"fees.view"))
            {
                btnFeeStructures->Enabled = false;
                btnStudentCharges->Enabled = false;
                btnRecordPayment->Enabled = false;
                btnPaymentHistory->Enabled = false;
            }
        }
    };
}

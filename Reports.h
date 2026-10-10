#pragma once

#include "DbConnection.h"
#include "AuthSession.h"
#include "ThemeManager.h"

#include <mariadb/conncpp.hpp>
#include <msclr/marshal_cppstd.h>
#include <memory>
#include <string>

namespace SchoolCore
{
    using namespace System;
    using namespace System::Drawing;
    using namespace System::IO;
    using namespace System::Text;
    using namespace System::Windows::Forms;

    public ref class Reports : public Form
    {
    private:
        ref class LookupItem
        {
        public:
            int Id;
            String^ Label;
            bool IsActive;

            LookupItem(int id, String^ label, bool active)
                : Id(id), Label(label), IsActive(active) {}

            virtual String^ ToString() override { return Label; }
        };

        ref class ReportOption
        {
        public:
            String^ Code;
            String^ Label;
            String^ Permission;
            bool UsesAcademicPeriod;
            bool UsesDateRange;

            ReportOption(String^ code, String^ label, String^ permission,
                bool academicPeriod, bool dateRange)
                : Code(code), Label(label), Permission(permission),
                  UsesAcademicPeriod(academicPeriod), UsesDateRange(dateRange) {}

            virtual String^ ToString() override { return Label; }
        };

        System::ComponentModel::Container^ components;
        TableLayoutPanel^ rootLayout;
        Label^ lblTitle;
        Label^ lblSubtitle;
        Label^ lblResultSummary;
        FlowLayoutPanel^ filterArea;
        ComboBox^ cmbReportType;
        ComboBox^ cmbAcademicYear;
        ComboBox^ cmbTerm;
        DateTimePicker^ dtpFrom;
        DateTimePicker^ dtpTo;
        Panel^ yearGroup;
        Panel^ termGroup;
        Panel^ fromGroup;
        Panel^ toGroup;
        Button^ btnGenerate;
        Button^ btnExport;
        Button^ btnBack;
        DataGridView^ reportGrid;
        bool loadingAcademicFilters = false;

    public:
        Reports(void)
        {
            InitializeComponent();
            ThemeManager::ApplyToForm(this);

            if (!AuthSession::HasPermission(L"reports.view"))
            {
                MessageBox::Show(L"You do not have permission to access Reports.",
                    L"Access Denied", MessageBoxButtons::OK, MessageBoxIcon::Warning);
                this->Close();
                return;
            }

            PopulateAvailableReports();
            LoadAcademicYears();

            this->cmbAcademicYear->SelectedIndexChanged +=
                gcnew EventHandler(this, &Reports::cmbAcademicYear_SelectedIndexChanged);
            LoadTerms();

            this->cmbReportType->SelectedIndexChanged +=
                gcnew EventHandler(this, &Reports::cmbReportType_SelectedIndexChanged);
            this->btnGenerate->Click +=
                gcnew EventHandler(this, &Reports::btnGenerate_Click);
            this->btnExport->Click +=
                gcnew EventHandler(this, &Reports::btnExport_Click);
            this->btnBack->Click +=
                gcnew EventHandler(this, &Reports::btnBack_Click);

            ApplyReportFilterVisibility();
            this->lblResultSummary->Text =
                L"Choose a report and its filters, then select Generate.";
        }

    protected:
        ~Reports(void)
        {
            if (this->components != nullptr)
                delete this->components;
        }

    private:
        void InitializeComponent()
        {
            this->SuspendLayout();

            this->Text = L"SchoolCore - Reports";
            this->Name = L"Reports";
            this->BackColor = Color::FromArgb(248, 250, 252);
            this->ForeColor = Color::FromArgb(15, 23, 42);
            this->Font = gcnew Drawing::Font(L"Segoe UI", 9.5F);
            this->ClientSize = Drawing::Size(1150, 720);
            this->MinimumSize = Drawing::Size(780, 560);
            this->StartPosition = FormStartPosition::CenterParent;
            this->DoubleBuffered = true;

            rootLayout = gcnew TableLayoutPanel();
            rootLayout->Dock = DockStyle::Fill;
            rootLayout->Padding = Padding(24);
            rootLayout->ColumnCount = 1;
            rootLayout->RowCount = 5;
            rootLayout->BackColor = Color::FromArgb(248, 250, 252);
            rootLayout->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Percent, 100.0F));
            rootLayout->RowStyles->Add(gcnew RowStyle(SizeType::Absolute, 44.0F));
            rootLayout->RowStyles->Add(gcnew RowStyle(SizeType::Absolute, 30.0F));
            rootLayout->RowStyles->Add(gcnew RowStyle(SizeType::Absolute, 142.0F));
            rootLayout->RowStyles->Add(gcnew RowStyle(SizeType::Absolute, 30.0F));
            rootLayout->RowStyles->Add(gcnew RowStyle(SizeType::Percent, 100.0F));

            lblTitle = gcnew Label();
            lblTitle->Text = L"Reports Centre";
            lblTitle->Dock = DockStyle::Fill;
            lblTitle->Font = gcnew Drawing::Font(L"Segoe UI Semibold", 22.0F, FontStyle::Bold);
            lblTitle->ForeColor = Color::FromArgb(15, 23, 42);
            lblTitle->TextAlign = ContentAlignment::MiddleLeft;

            lblSubtitle = gcnew Label();
            lblSubtitle->Text = L"Generate read-only school reports and export them to CSV.";
            lblSubtitle->Dock = DockStyle::Fill;
            lblSubtitle->Font = gcnew Drawing::Font(L"Segoe UI", 9.5F);
            lblSubtitle->ForeColor = Color::FromArgb(100, 116, 139);
            lblSubtitle->TextAlign = ContentAlignment::MiddleLeft;

            filterArea = gcnew FlowLayoutPanel();
            filterArea->Dock = DockStyle::Fill;
            filterArea->FlowDirection = FlowDirection::LeftToRight;
            filterArea->WrapContents = true;
            filterArea->AutoScroll = true;
            filterArea->Padding = Padding(0, 4, 0, 4);
            filterArea->BackColor = Color::FromArgb(248, 250, 252);

            cmbReportType = gcnew ComboBox();
            cmbReportType->DropDownStyle = ComboBoxStyle::DropDownList;
            cmbReportType->Font = gcnew Drawing::Font(L"Segoe UI", 9.5F);
            cmbReportType->Width = 230;

            cmbAcademicYear = gcnew ComboBox();
            cmbAcademicYear->DropDownStyle = ComboBoxStyle::DropDownList;
            cmbAcademicYear->Font = gcnew Drawing::Font(L"Segoe UI", 9.5F);
            cmbAcademicYear->Width = 145;

            cmbTerm = gcnew ComboBox();
            cmbTerm->DropDownStyle = ComboBoxStyle::DropDownList;
            cmbTerm->Font = gcnew Drawing::Font(L"Segoe UI", 9.5F);
            cmbTerm->Width = 125;

            dtpFrom = gcnew DateTimePicker();
            dtpFrom->Format = DateTimePickerFormat::Short;
            dtpFrom->Font = gcnew Drawing::Font(L"Segoe UI", 9.5F);
            dtpFrom->Value = DateTime::Now.Date.AddDays(-30);

            dtpTo = gcnew DateTimePicker();
            dtpTo->Format = DateTimePickerFormat::Short;
            dtpTo->Font = gcnew Drawing::Font(L"Segoe UI", 9.5F);
            dtpTo->Value = DateTime::Now.Date;

            yearGroup = CreateFilterGroup(L"Academic Year", cmbAcademicYear, 155);
            termGroup = CreateFilterGroup(L"Term", cmbTerm, 135);
            fromGroup = CreateFilterGroup(L"From Date", dtpFrom, 142);
            toGroup = CreateFilterGroup(L"To Date", dtpTo, 142);

            filterArea->Controls->Add(CreateFilterGroup(L"Report Type", cmbReportType, 240));
            filterArea->Controls->Add(yearGroup);
            filterArea->Controls->Add(termGroup);
            filterArea->Controls->Add(fromGroup);
            filterArea->Controls->Add(toGroup);

            Panel^ actionGroup = gcnew Panel();
            actionGroup->Width = 350;
            actionGroup->Height = 68;
            actionGroup->Margin = Padding(0, 4, 8, 4);

            Label^ actionLabel = gcnew Label();
            actionLabel->Text = L"Actions";
            actionLabel->Location = Point(0, 0);
            actionLabel->Size = Drawing::Size(200, 18);
            actionLabel->Font = gcnew Drawing::Font(L"Segoe UI", 8.5F);
            actionLabel->ForeColor = Color::FromArgb(100, 116, 139);

            btnGenerate = gcnew Button();
            btnGenerate->Text = L"Generate";
            btnGenerate->Location = Point(0, 24);
            btnGenerate->Size = Drawing::Size(102, 34);
            btnGenerate->BackColor = Color::FromArgb(29, 78, 216);
            btnGenerate->ForeColor = Color::White;
            btnGenerate->FlatStyle = FlatStyle::Flat;
            btnGenerate->FlatAppearance->BorderSize = 0;
            btnGenerate->Cursor = Cursors::Hand;

            btnExport = gcnew Button();
            btnExport->Text = L"Export CSV";
            btnExport->Location = Point(112, 24);
            btnExport->Size = Drawing::Size(105, 34);
            btnExport->BackColor = Color::FromArgb(226, 232, 240);
            btnExport->ForeColor = Color::FromArgb(15, 23, 42);
            btnExport->FlatStyle = FlatStyle::Flat;
            btnExport->FlatAppearance->BorderSize = 1;
            btnExport->FlatAppearance->BorderColor = Color::FromArgb(203, 213, 225);
            btnExport->Cursor = Cursors::Hand;
            btnExport->Enabled = false;

            btnBack = gcnew Button();
            btnBack->Text = L"Back";
            btnBack->Location = Point(227, 24);
            btnBack->Size = Drawing::Size(95, 34);
            btnBack->BackColor = Color::White;
            btnBack->ForeColor = Color::FromArgb(15, 23, 42);
            btnBack->FlatStyle = FlatStyle::Flat;
            btnBack->FlatAppearance->BorderSize = 1;
            btnBack->FlatAppearance->BorderColor = Color::FromArgb(203, 213, 225);
            btnBack->Cursor = Cursors::Hand;

            actionGroup->Controls->Add(actionLabel);
            actionGroup->Controls->Add(btnGenerate);
            actionGroup->Controls->Add(btnExport);
            actionGroup->Controls->Add(btnBack);
            filterArea->Controls->Add(actionGroup);

            lblResultSummary = gcnew Label();
            lblResultSummary->Dock = DockStyle::Fill;
            lblResultSummary->Text = L"No report generated.";
            lblResultSummary->ForeColor = Color::FromArgb(71, 85, 105);
            lblResultSummary->TextAlign = ContentAlignment::MiddleLeft;
            lblResultSummary->Font = gcnew Drawing::Font(L"Segoe UI", 9.0F);

            reportGrid = gcnew DataGridView();
            reportGrid->Name = L"reportGrid";
            reportGrid->Dock = DockStyle::Fill;
            reportGrid->ReadOnly = true;
            reportGrid->AllowUserToAddRows = false;
            reportGrid->AllowUserToDeleteRows = false;
            reportGrid->AllowUserToOrderColumns = true;
            reportGrid->AllowUserToResizeRows = false;
            reportGrid->MultiSelect = false;
            reportGrid->SelectionMode = DataGridViewSelectionMode::FullRowSelect;
            reportGrid->RowHeadersVisible = false;
            reportGrid->AutoGenerateColumns = false;
            reportGrid->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::DisplayedCells;
            reportGrid->BackgroundColor = Color::White;
            reportGrid->BorderStyle = BorderStyle::None;
            reportGrid->GridColor = Color::FromArgb(226, 232, 240);
            reportGrid->ColumnHeadersHeight = 38;
            reportGrid->EnableHeadersVisualStyles = false;

            rootLayout->Controls->Add(lblTitle, 0, 0);
            rootLayout->Controls->Add(lblSubtitle, 0, 1);
            rootLayout->Controls->Add(filterArea, 0, 2);
            rootLayout->Controls->Add(lblResultSummary, 0, 3);
            rootLayout->Controls->Add(reportGrid, 0, 4);

            this->Controls->Add(rootLayout);
            this->AcceptButton = btnGenerate;
            this->CancelButton = btnBack;

            this->ResumeLayout(false);
            this->PerformLayout();
        }

        Panel^ CreateFilterGroup(String^ caption, Control^ input, int width)
        {
            Panel^ group = gcnew Panel();
            group->Width = width;
            group->Height = 68;
            group->Margin = Padding(0, 4, 12, 4);

            Label^ label = gcnew Label();
            label->Text = caption;
            label->Location = Point(0, 0);
            label->Size = Drawing::Size(width - 4, 18);
            label->Font = gcnew Drawing::Font(L"Segoe UI", 8.5F);
            label->ForeColor = Color::FromArgb(100, 116, 139);

            input->Location = Point(0, 23);
            input->Size = Drawing::Size(width - 6, 30);
            input->Anchor = AnchorStyles::Top | AnchorStyles::Left | AnchorStyles::Right;

            group->Controls->Add(label);
            group->Controls->Add(input);
            return group;
        }

        void AddReportOption(String^ code, String^ label, String^ permission,
            bool academicPeriod, bool dateRange)
        {
            if (AuthSession::HasPermission(permission))
                cmbReportType->Items->Add(
                    gcnew ReportOption(code, label, permission, academicPeriod, dateRange));
        }

        void PopulateAvailableReports()
        {
            cmbReportType->Items->Clear();

            AddReportOption(L"enrolments", L"Student Enrolments", L"students.view", true, false);
            AddReportOption(L"attendance", L"Attendance Summary", L"attendance.view", true, true);
            AddReportOption(L"examinations", L"Examination Schedule", L"examinations.view", true, false);
            AddReportOption(L"payments", L"Fee Payments", L"fees.view", false, true);

            // Audit entries may contain sensitive operational details.
            if (AuthSession::HasPermission(L"users.view"))
                AddReportOption(L"audit", L"System Audit Trail", L"users.view", false, true);

            if (cmbReportType->Items->Count > 0)
                cmbReportType->SelectedIndex = 0;

            btnGenerate->Enabled = cmbReportType->Items->Count > 0;

            if (cmbReportType->Items->Count == 0)
                lblResultSummary->Text =
                    L"No reports are available for your current permissions.";
        }

        void LoadAcademicYears()
        {
            loadingAcademicFilters = true;
            cmbAcademicYear->BeginUpdate();
            cmbAcademicYear->Items->Clear();

            try
            {
                auto con = DbConnection::GetConnection();
                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT academic_year_id, year_name, status "
                        "FROM academic_years "
                        "ORDER BY CASE WHEN status = 'Active' THEN 0 ELSE 1 END, "
                        "academic_year_id DESC"));
                std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());
                int activeIndex = -1;

                while (result->next())
                {
                    int id = result->getInt("academic_year_id");
                    String^ label = gcnew String(result->getString("year_name").c_str());
                    bool active = String::Equals(
                        gcnew String(result->getString("status").c_str()),
                        L"Active", StringComparison::OrdinalIgnoreCase);

                    cmbAcademicYear->Items->Add(gcnew LookupItem(id, label, active));
                    if (active)
                        activeIndex = cmbAcademicYear->Items->Count - 1;
                }

                if (cmbAcademicYear->Items->Count > 0)
                    cmbAcademicYear->SelectedIndex = activeIndex >= 0 ? activeIndex : 0;
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(gcnew String(ex.what()), L"Reports - Academic Years",
                    MessageBoxButtons::OK, MessageBoxIcon::Error);
            }

            cmbAcademicYear->EndUpdate();
            loadingAcademicFilters = false;
        }

        void LoadTerms()
        {
            loadingAcademicFilters = true;
            cmbTerm->BeginUpdate();
            cmbTerm->Items->Clear();

            LookupItem^ year = dynamic_cast<LookupItem^>(cmbAcademicYear->SelectedItem);
            if (year == nullptr)
            {
                cmbTerm->EndUpdate();
                loadingAcademicFilters = false;
                return;
            }

            try
            {
                auto con = DbConnection::GetConnection();
                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT term_id, term_name, status "
                        "FROM terms WHERE academic_year_id = ? "
                        "ORDER BY CASE WHEN status = 'Active' THEN 0 ELSE 1 END, term_id"));
                stmt->setInt(1, year->Id);
                std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());
                int activeIndex = -1;

                while (result->next())
                {
                    int id = result->getInt("term_id");
                    String^ label = gcnew String(result->getString("term_name").c_str());
                    bool active = String::Equals(
                        gcnew String(result->getString("status").c_str()),
                        L"Active", StringComparison::OrdinalIgnoreCase);

                    cmbTerm->Items->Add(gcnew LookupItem(id, label, active));
                    if (active)
                        activeIndex = cmbTerm->Items->Count - 1;
                }

                if (cmbTerm->Items->Count > 0)
                    cmbTerm->SelectedIndex = activeIndex >= 0 ? activeIndex : 0;
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(gcnew String(ex.what()), L"Reports - Terms",
                    MessageBoxButtons::OK, MessageBoxIcon::Error);
            }

            cmbTerm->EndUpdate();
            loadingAcademicFilters = false;
        }

        void cmbAcademicYear_SelectedIndexChanged(Object^ sender, EventArgs^ e)
        {
            if (!loadingAcademicFilters)
                LoadTerms();
        }

        void cmbReportType_SelectedIndexChanged(Object^ sender, EventArgs^ e)
        {
            ApplyReportFilterVisibility();
        }

        void ApplyReportFilterVisibility()
        {
            ReportOption^ option = dynamic_cast<ReportOption^>(cmbReportType->SelectedItem);
            bool hasOption = option != nullptr;
            yearGroup->Visible = hasOption && option->UsesAcademicPeriod;
            termGroup->Visible = hasOption && option->UsesAcademicPeriod;
            fromGroup->Visible = hasOption && option->UsesDateRange;
            toGroup->Visible = hasOption && option->UsesDateRange;
            btnGenerate->Enabled = hasOption;
            filterArea->PerformLayout();
        }

        String^ GetResultValue(sql::ResultSet* result, unsigned int columnIndex)
        {
            if (result->isNull(columnIndex))
                return L"";
            return gcnew String(result->getString(columnIndex).c_str());
        }

        void LoadResultIntoGrid(sql::ResultSet* result)
        {
            reportGrid->Rows->Clear();
            reportGrid->Columns->Clear();

            std::unique_ptr<sql::ResultSetMetaData> metadata(result->getMetaData());
            unsigned int columnCount = metadata->getColumnCount();

            for (unsigned int i = 1; i <= columnCount; i++)
            {
                String^ heading = gcnew String(metadata->getColumnLabel(i).c_str());
                DataGridViewTextBoxColumn^ column = gcnew DataGridViewTextBoxColumn();
                column->Name = L"ReportColumn" + i.ToString();
                column->HeaderText = heading;
                column->ReadOnly = true;
                column->SortMode = DataGridViewColumnSortMode::Automatic;
                column->MinimumWidth = 85;
                reportGrid->Columns->Add(column);
            }

            while (result->next())
            {
                array<Object^>^ values =
                    gcnew array<Object^>(static_cast<int>(columnCount));
                for (unsigned int i = 1; i <= columnCount; i++)
                    values[static_cast<int>(i - 1)] = GetResultValue(result, i);
                reportGrid->Rows->Add(values);
            }

            reportGrid->ClearSelection();
            btnExport->Enabled = true;
        }

        bool TryGetAcademicPeriod(int% yearId, int% termId)
        {
            LookupItem^ year = dynamic_cast<LookupItem^>(cmbAcademicYear->SelectedItem);
            LookupItem^ term = dynamic_cast<LookupItem^>(cmbTerm->SelectedItem);

            if (year == nullptr || term == nullptr)
            {
                MessageBox::Show(
                    L"Select an academic year and term that have been configured in SchoolCore.",
                    L"Academic Period Required", MessageBoxButtons::OK, MessageBoxIcon::Information);
                return false;
            }

            yearId = year->Id;
            termId = term->Id;
            return true;
        }

        void btnGenerate_Click(Object^ sender, EventArgs^ e)
        {
            ReportOption^ option = dynamic_cast<ReportOption^>(cmbReportType->SelectedItem);
            if (option == nullptr)
                return;

            if (!AuthSession::HasPermission(L"reports.view") ||
                !AuthSession::HasPermission(option->Permission))
            {
                MessageBox::Show(L"You do not have permission to generate this report.",
                    L"Access Denied", MessageBoxButtons::OK, MessageBoxIcon::Warning);
                return;
            }

            int yearId = 0;
            int termId = 0;
            if (option->UsesAcademicPeriod && !TryGetAcademicPeriod(yearId, termId))
                return;

            if (option->UsesDateRange && dtpFrom->Value.Date > dtpTo->Value.Date)
            {
                MessageBox::Show(L"The From Date cannot be later than the To Date.",
                    L"Check Date Range", MessageBoxButtons::OK, MessageBoxIcon::Information);
                return;
            }

            reportGrid->Rows->Clear();
            reportGrid->Columns->Clear();
            btnExport->Enabled = false;
            lblResultSummary->Text = L"Generating report...";

            try
            {
                auto con = DbConnection::GetConnection();
                std::unique_ptr<sql::PreparedStatement> stmt;

                if (option->Code->Equals(L"enrolments", StringComparison::Ordinal))
                {
                    stmt.reset(con->prepareStatement(
                        "SELECT s.registration_number AS 'Registration No.', "
                        "CONCAT_WS(' ', s.first_name, NULLIF(s.middle_name, ''), s.last_name) AS 'Student Name', "
                        "COALESCE(s.gender, '') AS Gender, c.class_name AS Class, "
                        "COALESCE(st.stream_name, 'Not assigned') AS Stream, "
                        "ay.year_name AS 'Academic Year', t.term_name AS Term, e.status AS Status "
                        "FROM enrollments e INNER JOIN students s ON s.student_id = e.student_id "
                        "INNER JOIN classes c ON c.class_id = e.class_id "
                        "LEFT JOIN streams st ON st.stream_id = e.stream_id "
                        "INNER JOIN academic_years ay ON ay.academic_year_id = e.academic_year_id "
                        "INNER JOIN terms t ON t.term_id = e.term_id "
                        "WHERE e.academic_year_id = ? AND e.term_id = ? "
                        "ORDER BY c.class_name, st.stream_name, s.last_name, s.first_name"));
                    stmt->setInt(1, yearId);
                    stmt->setInt(2, termId);
                }
                else if (option->Code->Equals(L"attendance", StringComparison::Ordinal))
                {
                    stmt.reset(con->prepareStatement(
                        "SELECT DATE_FORMAT(a.attendance_date, '%Y-%m-%d') AS Date, "
                        "c.class_name AS Class, COALESCE(st.stream_name, 'All streams') AS Stream, "
                        "COUNT(ar.attendance_record_id) AS 'Students Marked', "
                        "COALESCE(SUM(CASE WHEN LOWER(ar.attendance_status) = 'present' THEN 1 ELSE 0 END), 0) AS Present, "
                        "COALESCE(SUM(CASE WHEN LOWER(ar.attendance_status) = 'absent' THEN 1 ELSE 0 END), 0) AS Absent, "
                        "COALESCE(SUM(CASE WHEN LOWER(ar.attendance_status) = 'late' THEN 1 ELSE 0 END), 0) AS Late "
                        "FROM attendance_sessions a INNER JOIN classes c ON c.class_id = a.class_id "
                        "LEFT JOIN streams st ON st.stream_id = a.stream_id "
                        "LEFT JOIN attendance_records ar ON ar.attendance_session_id = a.attendance_session_id "
                        "WHERE a.academic_year_id = ? AND a.term_id = ? "
                        "AND a.attendance_date BETWEEN ? AND ? "
                        "GROUP BY a.attendance_session_id, a.attendance_date, c.class_name, st.stream_name "
                        "ORDER BY a.attendance_date DESC, c.class_name"));
                    stmt->setInt(1, yearId);
                    stmt->setInt(2, termId);
                    stmt->setString(3, msclr::interop::marshal_as<std::string>(
                        dtpFrom->Value.ToString(L"yyyy-MM-dd")));
                    stmt->setString(4, msclr::interop::marshal_as<std::string>(
                        dtpTo->Value.ToString(L"yyyy-MM-dd")));
                }
                else if (option->Code->Equals(L"examinations", StringComparison::Ordinal))
                {
                    stmt.reset(con->prepareStatement(
                        "SELECT e.examination_name AS Examination, "
                        "COALESCE(e.examination_type, '') AS Type, c.class_name AS Class, "
                        "ay.year_name AS 'Academic Year', t.term_name AS Term, "
                        "COALESCE(DATE_FORMAT(e.start_date, '%Y-%m-%d'), '') AS 'Start Date', "
                        "COALESCE(DATE_FORMAT(e.end_date, '%Y-%m-%d'), '') AS 'End Date', "
                        "e.status AS Status FROM examinations e "
                        "INNER JOIN classes c ON c.class_id = e.class_id "
                        "INNER JOIN academic_years ay ON ay.academic_year_id = e.academic_year_id "
                        "INNER JOIN terms t ON t.term_id = e.term_id "
                        "WHERE e.academic_year_id = ? AND e.term_id = ? "
                        "ORDER BY e.start_date DESC, e.examination_name"));
                    stmt->setInt(1, yearId);
                    stmt->setInt(2, termId);
                }
                else if (option->Code->Equals(L"payments", StringComparison::Ordinal))
                {
                    stmt.reset(con->prepareStatement(
                        "SELECT DATE_FORMAT(p.payment_date, '%Y-%m-%d') AS Date, "
                        "p.receipt_number AS Receipt, s.registration_number AS 'Registration No.', "
                        "CONCAT_WS(' ', s.first_name, NULLIF(s.middle_name, ''), s.last_name) AS Student, "
                        "COALESCE(p.payment_method, '') AS Method, "
                        "COALESCE(p.transaction_reference, '') AS Reference, FORMAT(p.amount, 2) AS 'Amount (UGX)', "
                        "COALESCE(u.full_name, u.username, '') AS 'Received By' "
                        "FROM payments p INNER JOIN students s ON s.student_id = p.student_id "
                        "LEFT JOIN users u ON u.user_id = p.received_by "
                        "WHERE p.payment_date BETWEEN ? AND ? "
                        "ORDER BY p.payment_date DESC, p.payment_id DESC"));
                    stmt->setString(1, msclr::interop::marshal_as<std::string>(
                        dtpFrom->Value.ToString(L"yyyy-MM-dd")));
                    stmt->setString(2, msclr::interop::marshal_as<std::string>(
                        dtpTo->Value.ToString(L"yyyy-MM-dd")));
                }
                else if (option->Code->Equals(L"audit", StringComparison::Ordinal))
                {
                    stmt.reset(con->prepareStatement(
                        "SELECT DATE_FORMAT(a.created_at, '%Y-%m-%d %H:%i:%s') AS 'Date and Time', "
                        "COALESCE(u.full_name, u.username, 'System') AS 'User', "
                        "a.action AS Action, COALESCE(a.table_name, '') AS 'Affected Table', "
                        "COALESCE(a.record_id, '') AS 'Record ID', COALESCE(a.description, '') AS Description "
                        "FROM audit_logs a LEFT JOIN users u ON u.user_id = a.user_id "
                        "WHERE a.created_at >= ? AND a.created_at < DATE_ADD(?, INTERVAL 1 DAY) "
                        "ORDER BY a.created_at DESC LIMIT 500"));
                    stmt->setString(1, msclr::interop::marshal_as<std::string>(
                        dtpFrom->Value.ToString(L"yyyy-MM-dd")));
                    stmt->setString(2, msclr::interop::marshal_as<std::string>(
                        dtpTo->Value.ToString(L"yyyy-MM-dd")));
                }
                else
                {
                    MessageBox::Show(L"Select a supported report.", L"Reports",
                        MessageBoxButtons::OK, MessageBoxIcon::Information);
                    return;
                }

                std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());
                LoadResultIntoGrid(result.get());

                lblResultSummary->Text =
                    option->Label + L" | " +
                    reportGrid->Rows->Count.ToString() + L" record(s)";
            }
            catch (sql::SQLException& ex)
            {
                reportGrid->Rows->Clear();
                reportGrid->Columns->Clear();
                btnExport->Enabled = false;
                lblResultSummary->Text = L"The report could not be generated.";
                MessageBox::Show(gcnew String(ex.what()), L"Report Error",
                    MessageBoxButtons::OK, MessageBoxIcon::Error);
            }
            catch (Exception^ ex)
            {
                reportGrid->Rows->Clear();
                reportGrid->Columns->Clear();
                btnExport->Enabled = false;
                lblResultSummary->Text = L"The report could not be generated.";
                MessageBox::Show(ex->Message, L"Report Error",
                    MessageBoxButtons::OK, MessageBoxIcon::Error);
            }
        }

        String^ EscapeCsv(String^ value)
        {
            if (value == nullptr)
                value = L"";
            return L"\"" + value->Replace(L"\"", L"\"\"") + L"\"";
        }

        void btnExport_Click(Object^ sender, EventArgs^ e)
        {
            if (reportGrid->Columns->Count == 0)
            {
                MessageBox::Show(L"Generate a report before exporting.", L"Export CSV",
                    MessageBoxButtons::OK, MessageBoxIcon::Information);
                return;
            }

            SaveFileDialog^ dialog = gcnew SaveFileDialog();
            dialog->Title = L"Export SchoolCore Report";
            dialog->Filter = L"CSV files (*.csv)|*.csv";
            dialog->DefaultExt = L"csv";
            dialog->AddExtension = true;
            dialog->FileName =
                L"SchoolCore_Report_" + DateTime::Now.ToString(L"yyyyMMdd_HHmm") + L".csv";

            if (dialog->ShowDialog(this) != DialogResult::OK)
            {
                delete dialog;
                return;
            }

            try
            {
                StringBuilder^ output = gcnew StringBuilder();

                for (int i = 0; i < reportGrid->Columns->Count; i++)
                {
                    if (i > 0)
                        output->Append(L",");
                    output->Append(EscapeCsv(reportGrid->Columns[i]->HeaderText));
                }
                output->AppendLine();

                for each (DataGridViewRow^ row in reportGrid->Rows)
                {
                    if (row->IsNewRow)
                        continue;

                    for (int i = 0; i < reportGrid->Columns->Count; i++)
                    {
                        if (i > 0)
                            output->Append(L",");

                        Object^ cellValue = row->Cells[i]->Value;
                        output->Append(EscapeCsv(
                            cellValue == nullptr ? L"" : cellValue->ToString()));
                    }
                    output->AppendLine();
                }

                File::WriteAllText(dialog->FileName, output->ToString(), gcnew UTF8Encoding(true));
                MessageBox::Show(L"Report exported successfully.", L"Export CSV",
                    MessageBoxButtons::OK, MessageBoxIcon::Information);
            }
            catch (Exception^ ex)
            {
                MessageBox::Show(L"The report could not be exported.\n" + ex->Message,
                    L"Export CSV", MessageBoxButtons::OK, MessageBoxIcon::Error);
            }

            delete dialog;
        }

        void btnBack_Click(Object^ sender, EventArgs^ e)
        {
            this->Close();
        }
    };
}

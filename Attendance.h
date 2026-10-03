#pragma once

#include "DbConnection.h"
#include "AuthSession.h"

#include <mariadb/conncpp.hpp>
#include <msclr/marshal_cppstd.h>
#include <memory>
#include <string>

using namespace System;
using namespace System::Drawing;
using namespace System::Windows::Forms;

namespace SchoolCore
{
    public ref class Attendance : public Form
    {
    private:
        TableLayoutPanel^ mainLayout;
        Panel^ headerPanel;
        Label^ lblTitle;
        Label^ lblSubtitle;

        Panel^ filterPanel;
        Label^ lblYear;
        Label^ lblTerm;
        Label^ lblClass;
        Label^ lblStream;
        Label^ lblDate;

        ComboBox^ cmbAcademicYear;
        ComboBox^ cmbTerm;
        ComboBox^ cmbClass;
        ComboBox^ cmbStream;
        DateTimePicker^ dtpAttendanceDate;

        Label^ lblSessionInfo;
        Button^ btnLoadStudents;
        Button^ btnSaveAttendance;
        Button^ btnRefresh;
        Button^ btnBack;

        DataGridView^ attendanceGrid;
        Label^ lblStudentCount;
        Label^ lblStatus;

        void StyleCombo(ComboBox^ combo)
        {
            combo->DropDownStyle = ComboBoxStyle::DropDownList;
            combo->Font = gcnew System::Drawing::Font(
                L"Segoe UI", 9.5F, FontStyle::Regular
            );
            combo->Height = 32;
        }

        Label^ CreateFilterLabel(String^ text)
        {
            Label^ label = gcnew Label();
            label->Text = text;
            label->Dock = DockStyle::Fill;
            label->TextAlign = ContentAlignment::MiddleLeft;
            label->Font = gcnew System::Drawing::Font(
                L"Segoe UI Semibold", 9.0F, FontStyle::Bold
            );
            label->ForeColor = Color::FromArgb(30, 41, 59);
            return label;
        }

        void InitializeComponent()
        {
            this->SuspendLayout();

            this->Text = L"Attendance";
            this->StartPosition = FormStartPosition::CenterScreen;
            this->WindowState = FormWindowState::Maximized;
            this->MinimumSize = System::Drawing::Size(1000, 680);
            this->BackColor = Color::FromArgb(248, 250, 252);

            this->mainLayout = gcnew TableLayoutPanel();
            this->mainLayout->Dock = DockStyle::Fill;
            this->mainLayout->ColumnCount = 1;
            this->mainLayout->RowCount = 4;
            this->mainLayout->Padding = System::Windows::Forms::Padding(20);
            this->mainLayout->BackColor = Color::FromArgb(248, 250, 252);

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 82.0F)
            );
            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 126.0F)
            );
            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(SizeType::Percent, 100.0F)
            );
            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 58.0F)
            );

            // Header
            this->headerPanel = gcnew Panel();
            this->headerPanel->Dock = DockStyle::Fill;
            this->headerPanel->BackColor = Color::FromArgb(30, 41, 59);
            this->headerPanel->Padding =
                System::Windows::Forms::Padding(20, 9, 20, 8);

            this->lblTitle = gcnew Label();
            this->lblTitle->Dock = DockStyle::Top;
            this->lblTitle->Height = 38;
            this->lblTitle->Text = L"Attendance";
            this->lblTitle->ForeColor = Color::White;
            this->lblTitle->Font = gcnew System::Drawing::Font(
                L"Segoe UI Semibold", 17.0F, FontStyle::Bold
            );
            this->lblTitle->TextAlign = ContentAlignment::MiddleLeft;

            this->lblSubtitle = gcnew Label();
            this->lblSubtitle->Dock = DockStyle::Fill;
            this->lblSubtitle->Text =
                L"Record daily student attendance by academic period, class and stream.";
            this->lblSubtitle->ForeColor = Color::Gainsboro;
            this->lblSubtitle->Font = gcnew System::Drawing::Font(
                L"Segoe UI", 9.5F, FontStyle::Regular
            );
            this->lblSubtitle->TextAlign = ContentAlignment::MiddleLeft;

            this->headerPanel->Controls->Add(this->lblSubtitle);
            this->headerPanel->Controls->Add(this->lblTitle);

            // Filters
            this->filterPanel = gcnew Panel();
            this->filterPanel->Dock = DockStyle::Fill;
            this->filterPanel->BackColor = Color::White;
            this->filterPanel->Padding =
                System::Windows::Forms::Padding(15, 10, 15, 10);

            TableLayoutPanel^ filterLayout = gcnew TableLayoutPanel();
            filterLayout->Dock = DockStyle::Fill;
            filterLayout->ColumnCount = 10;
            filterLayout->RowCount = 2;
            filterLayout->Padding = System::Windows::Forms::Padding(0);

            filterLayout->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Absolute, 105.0F));
            filterLayout->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Percent, 20.0F));
            filterLayout->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Absolute, 75.0F));
            filterLayout->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Percent, 20.0F));
            filterLayout->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Absolute, 65.0F));
            filterLayout->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Percent, 20.0F));
            filterLayout->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Absolute, 65.0F));
            filterLayout->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Percent, 20.0F));
            filterLayout->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Absolute, 55.0F));
            filterLayout->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Absolute, 130.0F));

            filterLayout->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 42.0F));
            filterLayout->RowStyles->Add(
                gcnew RowStyle(SizeType::Percent, 100.0F));

            this->cmbAcademicYear = gcnew ComboBox();
            this->cmbTerm = gcnew ComboBox();
            this->cmbClass = gcnew ComboBox();
            this->cmbStream = gcnew ComboBox();
            this->dtpAttendanceDate = gcnew DateTimePicker();

            StyleCombo(this->cmbAcademicYear);
            StyleCombo(this->cmbTerm);
            StyleCombo(this->cmbClass);
            StyleCombo(this->cmbStream);

            this->dtpAttendanceDate->Format = DateTimePickerFormat::Short;
            this->dtpAttendanceDate->Font = gcnew System::Drawing::Font(
                L"Segoe UI", 9.5F, FontStyle::Regular
            );
            this->dtpAttendanceDate->Dock = DockStyle::Fill;

            this->cmbAcademicYear->Items->Add(L"Select year");
            this->cmbTerm->Items->Add(L"Select term");
            this->cmbClass->Items->Add(L"Select class");
            this->cmbStream->Items->Add(L"All streams");

            this->cmbAcademicYear->SelectedIndex = 0;
            this->cmbTerm->SelectedIndex = 0;
            this->cmbClass->SelectedIndex = 0;
            this->cmbStream->SelectedIndex = 0;

            filterLayout->Controls->Add(
                CreateFilterLabel(L"Academic Year"), 0, 0);
            filterLayout->Controls->Add(
                this->cmbAcademicYear, 1, 0);
            filterLayout->Controls->Add(
                CreateFilterLabel(L"Term"), 2, 0);
            filterLayout->Controls->Add(
                this->cmbTerm, 3, 0);
            filterLayout->Controls->Add(
                CreateFilterLabel(L"Class"), 4, 0);
            filterLayout->Controls->Add(
                this->cmbClass, 5, 0);
            filterLayout->Controls->Add(
                CreateFilterLabel(L"Stream"), 6, 0);
            filterLayout->Controls->Add(
                this->cmbStream, 7, 0);
            filterLayout->Controls->Add(
                CreateFilterLabel(L"Date"), 8, 0);
            filterLayout->Controls->Add(
                this->dtpAttendanceDate, 9, 0);

            this->lblSessionInfo = gcnew Label();
            this->lblSessionInfo->Dock = DockStyle::Fill;
            this->lblSessionInfo->Text =
                L"Select the academic year, term and class, then load students.";
            this->lblSessionInfo->ForeColor = Color::DimGray;
            this->lblSessionInfo->TextAlign = ContentAlignment::MiddleLeft;
            this->lblSessionInfo->Font = gcnew System::Drawing::Font(
                L"Segoe UI", 9.0F, FontStyle::Regular
            );

            filterLayout->Controls->Add(
                this->lblSessionInfo, 0, 1);
            filterLayout->SetColumnSpan(this->lblSessionInfo, 8);

            this->btnLoadStudents = gcnew Button();
            this->btnLoadStudents->Text = L"Load Students";
            this->btnLoadStudents->Dock = DockStyle::Fill;
            this->btnLoadStudents->BackColor =
                Color::FromArgb(30, 41, 59);
            this->btnLoadStudents->ForeColor = Color::White;
            this->btnLoadStudents->FlatStyle = FlatStyle::Flat;
            this->btnLoadStudents->FlatAppearance->BorderSize = 0;
            this->btnLoadStudents->Cursor = Cursors::Hand;
            filterLayout->Controls->Add(
                this->btnLoadStudents, 8, 1);
            filterLayout->SetColumnSpan(
                this->btnLoadStudents, 2);

            this->filterPanel->Controls->Add(filterLayout);

            // Attendance grid
            this->attendanceGrid = gcnew DataGridView();
            this->attendanceGrid->Dock = DockStyle::Fill;
            this->attendanceGrid->BackgroundColor = Color::White;
            this->attendanceGrid->BorderStyle = BorderStyle::None;
            this->attendanceGrid->AllowUserToAddRows = false;
            this->attendanceGrid->AllowUserToDeleteRows = false;
            this->attendanceGrid->AllowUserToResizeRows = false;
            this->attendanceGrid->AutoGenerateColumns = false;
            this->attendanceGrid->ReadOnly = false;
            this->attendanceGrid->RowHeadersVisible = false;
            this->attendanceGrid->MultiSelect = false;
            this->attendanceGrid->SelectionMode =
                DataGridViewSelectionMode::FullRowSelect;
            this->attendanceGrid->ColumnHeadersHeight = 40;
            this->attendanceGrid->RowTemplate->Height = 36;
            this->attendanceGrid->EnableHeadersVisualStyles = false;
            this->attendanceGrid->ColumnHeadersDefaultCellStyle->BackColor =
                Color::FromArgb(30, 41, 59);
            this->attendanceGrid->ColumnHeadersDefaultCellStyle->ForeColor =
                Color::White;
            this->attendanceGrid->ColumnHeadersDefaultCellStyle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold", 9.5F, FontStyle::Bold);
            this->attendanceGrid->DefaultCellStyle->Font =
                gcnew System::Drawing.Font(
                    L"Segoe UI", 9.5F, FontStyle::Regular);
            this->attendanceGrid->DefaultCellStyle->SelectionBackColor =
                Color::FromArgb(219, 234, 254);
            this->attendanceGrid->DefaultCellStyle->SelectionForeColor =
                Color::FromArgb(30, 41, 59);
            this->attendanceGrid->AlternatingRowsDefaultCellStyle->BackColor =
                Color::FromArgb(248, 250, 252);

            DataGridViewTextBoxColumn^ studentIdColumn =
                gcnew DataGridViewTextBoxColumn();
            studentIdColumn->Name = L"StudentId";
            studentIdColumn->Visible = false;

            DataGridViewTextBoxColumn^ regColumn =
                gcnew DataGridViewTextBoxColumn();
            regColumn->Name = L"RegistrationNumber";
            regColumn->HeaderText = L"Registration No.";
            regColumn->Width = 170;
            regColumn->ReadOnly = true;

            DataGridViewTextBoxColumn^ nameColumn =
                gcnew DataGridViewTextBoxColumn();
            nameColumn->Name = L"StudentName";
            nameColumn->HeaderText = L"Student Name";
            nameColumn->AutoSizeMode =
                DataGridViewAutoSizeColumnMode::Fill;
            nameColumn->ReadOnly = true;

            DataGridViewComboBoxColumn^ statusColumn =
                gcnew DataGridViewComboBoxColumn();
            statusColumn->Name = L"AttendanceStatus";
            statusColumn->HeaderText = L"Attendance";
            statusColumn->Width = 150;
            statusColumn->FlatStyle = FlatStyle::Flat;
            statusColumn->Items->Add(L"Present");
            statusColumn->Items->Add(L"Absent");
            statusColumn->Items->Add(L"Late");
            statusColumn->Items->Add(L"Excused");

            DataGridViewTextBoxColumn^ remarksColumn =
                gcnew DataGridViewTextBoxColumn();
            remarksColumn->Name = L"Remarks";
            remarksColumn->HeaderText = L"Remarks";
            remarksColumn->Width = 240;

            this->attendanceGrid->Columns->Add(studentIdColumn);
            this->attendanceGrid->Columns->Add(regColumn);
            this->attendanceGrid->Columns->Add(nameColumn);
            this->attendanceGrid->Columns->Add(statusColumn);
            this->attendanceGrid->Columns->Add(remarksColumn);

            // Footer
            Panel^ footerPanel = gcnew Panel();
            footerPanel->Dock = DockStyle::Fill;
            footerPanel->BackColor = Color::White;
            footerPanel->Padding =
                System::Windows::Forms::Padding(12, 8, 12, 8);

            this->lblStudentCount = gcnew Label();
            this->lblStudentCount->Dock = DockStyle::Left;
            this->lblStudentCount->Width = 250;
            this->lblStudentCount->Text = L"Students: 0";
            this->lblStudentCount->ForeColor = Color::DimGray;
            this->lblStudentCount->TextAlign =
                ContentAlignment::MiddleLeft;

            FlowLayoutPanel^ actions = gcnew FlowLayoutPanel();
            actions->Dock = DockStyle::Right;
            actions->FlowDirection = FlowDirection::RightToLeft;
            actions->WrapContents = false;
            actions->AutoSize = true;

            this->btnBack = gcnew Button();
            this->btnBack->Text = L"Back to Dashboard";
            this->btnBack->Size = System::Drawing::Size(150, 40);

            this->btnRefresh = gcnew Button();
            this->btnRefresh->Text = L"Clear / Refresh";
            this->btnRefresh->Size = System::Drawing::Size(125, 40);

            this->btnSaveAttendance = gcnew Button();
            this->btnSaveAttendance->Text = L"Save Attendance";
            this->btnSaveAttendance->Size = System::Drawing::Size(145, 40);
            this->btnSaveAttendance->BackColor =
                Color::FromArgb(30, 41, 59);
            this->btnSaveAttendance->ForeColor = Color::White;
            this->btnSaveAttendance->FlatStyle = FlatStyle::Flat;
            this->btnSaveAttendance->FlatAppearance->BorderSize = 0;

            actions->Controls->Add(this->btnBack);
            actions->Controls->Add(this->btnRefresh);
            actions->Controls->Add(this->btnSaveAttendance);

            footerPanel->Controls->Add(actions);
            footerPanel->Controls->Add(this->lblStudentCount);

            this->mainLayout->Controls->Add(
                this->headerPanel, 0, 0);
            this->mainLayout->Controls->Add(
                this->filterPanel, 0, 1);
            this->mainLayout->Controls->Add(
                this->attendanceGrid, 0, 2);
            this->mainLayout->Controls->Add(
                footerPanel, 0, 3);

            this->Controls->Add(this->mainLayout);

            this->ResumeLayout(false);
        }

        void LoadAcademicYears()
        {
            this->cmbAcademicYear->Items->Clear();
            this->cmbAcademicYear->Items->Add(L"Select year");

            try
            {
                auto con = DbConnection::GetConnection();
                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT academic_year_id, year_name "
                        "FROM academic_years "
                        "WHERE status = 'Active' "
                        "ORDER BY academic_year_id DESC"
                    )
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                while (result->next())
                {
                    String^ name = gcnew String(
                        result->getString("year_name").c_str()
                    );

                    this->cmbAcademicYear->Items->Add(
                        gcnew FilterItem(
                            result->getInt("academic_year_id"),
                            name
                        )
                    );
                }

                this->cmbAcademicYear->SelectedIndex = 0;
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }

        void LoadTerms()
        {
            this->cmbTerm->Items->Clear();
            this->cmbTerm->Items->Add(L"Select term");
            this->cmbTerm->SelectedIndex = 0;

            if (this->cmbAcademicYear->SelectedIndex <= 0)
                return;

            FilterItem^ yearItem =
                dynamic_cast<FilterItem^>(
                    this->cmbAcademicYear->SelectedItem
                );

            if (yearItem == nullptr)
                return;

            try
            {
                auto con = DbConnection::GetConnection();
                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT term_id, term_name "
                        "FROM terms "
                        "WHERE academic_year_id = ? "
                        "AND status = 'Active' "
                        "ORDER BY term_id"
                    )
                );

                stmt->setInt(1, yearItem->Id);

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                while (result->next())
                {
                    this->cmbTerm->Items->Add(
                        gcnew FilterItem(
                            result->getInt("term_id"),
                            gcnew String(
                                result->getString("term_name").c_str()
                            )
                        )
                    );
                }
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }

        void LoadClasses()
        {
            this->cmbClass->Items->Clear();
            this->cmbClass->Items->Add(L"Select class");
            this->cmbClass->SelectedIndex = 0;

            try
            {
                auto con = DbConnection::GetConnection();
                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT class_id, class_name "
                        "FROM classes "
                        "WHERE status = 'Active' "
                        "ORDER BY class_name"
                    )
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                while (result->next())
                {
                    this->cmbClass->Items->Add(
                        gcnew FilterItem(
                            result->getInt("class_id"),
                            gcnew String(
                                result->getString("class_name").c_str()
                            )
                        )
                    );
                }
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }

        void LoadStreams()
        {
            this->cmbStream->Items->Clear();
            this->cmbStream->Items->Add(L"All streams");
            this->cmbStream->SelectedIndex = 0;

            if (this->cmbClass->SelectedIndex <= 0)
                return;

            FilterItem^ classItem =
                dynamic_cast<FilterItem^>(
                    this->cmbClass->SelectedItem
                );

            if (classItem == nullptr)
                return;

            try
            {
                auto con = DbConnection::GetConnection();
                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT stream_id, stream_name "
                        "FROM streams "
                        "WHERE class_id = ? "
                        "AND status = 'Active' "
                        "ORDER BY stream_name"
                    )
                );

                stmt->setInt(1, classItem->Id);

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                while (result->next())
                {
                    this->cmbStream->Items->Add(
                        gcnew FilterItem(
                            result->getInt("stream_id"),
                            gcnew String(
                                result->getString("stream_name").c_str()
                            )
                        )
                    );
                }
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }

        void ClearAttendanceGrid()
        {
            this->attendanceGrid->Rows->Clear();
            this->lblStudentCount->Text = L"Students: 0";
        }

        void LoadStudents()
        {
            ClearAttendanceGrid();

            if (this->cmbAcademicYear->SelectedIndex <= 0 ||
                this->cmbTerm->SelectedIndex <= 0 ||
                this->cmbClass->SelectedIndex <= 0)
            {
                MessageBox::Show(
                    L"Please select an academic year, term and class.",
                    L"Attendance",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                return;
            }

            FilterItem^ yearItem =
                dynamic_cast<FilterItem^>(
                    this->cmbAcademicYear->SelectedItem
                );
            FilterItem^ termItem =
                dynamic_cast<FilterItem^>(
                    this->cmbTerm->SelectedItem
                );
            FilterItem^ classItem =
                dynamic_cast<FilterItem^>(
                    this->cmbClass->SelectedItem
                );

            if (yearItem == nullptr ||
                termItem == nullptr ||
                classItem == nullptr)
                return;

            bool hasStream = this->cmbStream->SelectedIndex > 0;
            FilterItem^ streamItem = nullptr;

            if (hasStream)
            {
                streamItem =
                    dynamic_cast<FilterItem^>(
                        this->cmbStream->SelectedItem
                    );
            }

            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt;

                if (hasStream && streamItem != nullptr)
                {
                    stmt.reset(
                        con->prepareStatement(
                            "SELECT "
                            "s.student_id, "
                            "s.registration_number, "
                            "CONCAT_WS(' ', s.first_name, s.middle_name, s.last_name) AS student_name "
                            "FROM students s "
                            "INNER JOIN enrollments e "
                            "ON e.student_id = s.student_id "
                            "WHERE e.academic_year_id = ? "
                            "AND e.term_id = ? "
                            "AND e.class_id = ? "
                            "AND e.stream_id = ? "
                            "AND e.status = 'Active' "
                            "AND s.status = 'Active' "
                            "ORDER BY s.last_name, s.first_name"
                        )
                    );

                    stmt->setInt(1, yearItem->Id);
                    stmt->setInt(2, termItem->Id);
                    stmt->setInt(3, classItem->Id);
                    stmt->setInt(4, streamItem->Id);
                }
                else
                {
                    stmt.reset(
                        con->prepareStatement(
                            "SELECT "
                            "s.student_id, "
                            "s.registration_number, "
                            "CONCAT_WS(' ', s.first_name, s.middle_name, s.last_name) AS student_name "
                            "FROM students s "
                            "INNER JOIN enrollments e "
                            "ON e.student_id = s.student_id "
                            "WHERE e.academic_year_id = ? "
                            "AND e.term_id = ? "
                            "AND e.class_id = ? "
                            "AND e.status = 'Active' "
                            "AND s.status = 'Active' "
                            "ORDER BY s.last_name, s.first_name"
                        )
                    );

                    stmt->setInt(1, yearItem->Id);
                    stmt->setInt(2, termItem->Id);
                    stmt->setInt(3, classItem->Id);
                }

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                while (result->next())
                {
                    int rowIndex = this->attendanceGrid->Rows->Add();

                    this->attendanceGrid->Rows[rowIndex]
                        ->Cells["StudentId"]->Value =
                        result->getInt("student_id");

                    this->attendanceGrid->Rows[rowIndex]
                        ->Cells["RegistrationNumber"]->Value =
                        gcnew String(
                            result->getString(
                                "registration_number"
                            ).c_str()
                        );

                    this->attendanceGrid->Rows[rowIndex]
                        ->Cells["StudentName"]->Value =
                        gcnew String(
                            result->getString(
                                "student_name"
                            ).c_str()
                        );

                    this->attendanceGrid->Rows[rowIndex]
                        ->Cells["AttendanceStatus"]->Value =
                        L"Present";

                    this->attendanceGrid->Rows[rowIndex]
                        ->Cells["Remarks"]->Value = L"";
                }

                LoadExistingAttendance(
                    yearItem->Id,
                    termItem->Id,
                    classItem->Id,
                    hasStream && streamItem != nullptr
                        ? streamItem->Id
                        : 0
                );

                this->lblStudentCount->Text =
                    L"Students: " +
                    this->attendanceGrid->Rows->Count.ToString();

                this->lblSessionInfo->Text =
                    L"Attendance date: " +
                    this->dtpAttendanceDate->Value.ToString(L"d") +
                    L"  |  Mark each student, then save.";
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }

        void LoadExistingAttendance(
            int yearId,
            int termId,
            int classId,
            int streamId)
        {
            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> sessionStmt;

                if (streamId > 0)
                {
                    sessionStmt.reset(
                        con->prepareStatement(
                            "SELECT attendance_session_id "
                            "FROM attendance_sessions "
                            "WHERE attendance_date = ? "
                            "AND academic_year_id = ? "
                            "AND term_id = ? "
                            "AND class_id = ? "
                            "AND stream_id = ? "
                            "LIMIT 1"
                        )
                    );

                    sessionStmt->setString(
                        1,
                        msclr::interop::marshal_as<std::string>(
                            this->dtpAttendanceDate->Value.ToString(L"yyyy-MM-dd")
                        )
                    );
                    sessionStmt->setInt(2, yearId);
                    sessionStmt->setInt(3, termId);
                    sessionStmt->setInt(4, classId);
                    sessionStmt->setInt(5, streamId);
                }
                else
                {
                    sessionStmt.reset(
                        con->prepareStatement(
                            "SELECT attendance_session_id "
                            "FROM attendance_sessions "
                            "WHERE attendance_date = ? "
                            "AND academic_year_id = ? "
                            "AND term_id = ? "
                            "AND class_id = ? "
                            "AND stream_id IS NULL "
                            "LIMIT 1"
                        )
                    );

                    sessionStmt->setString(
                        1,
                        msclr::interop::marshal_as<std::string>(
                            this->dtpAttendanceDate->Value.ToString(L"yyyy-MM-dd")
                        )
                    );
                    sessionStmt->setInt(2, yearId);
                    sessionStmt->setInt(3, termId);
                    sessionStmt->setInt(4, classId);
                }

                std::unique_ptr<sql::ResultSet> sessionResult(
                    sessionStmt->executeQuery()
                );

                if (!sessionResult->next())
                    return;

                int sessionId =
                    sessionResult->getInt("attendance_session_id");

                std::unique_ptr<sql::PreparedStatement> recordStmt(
                    con->prepareStatement(
                        "SELECT student_id, attendance_status, remarks "
                        "FROM attendance_records "
                        "WHERE attendance_session_id = ?"
                    )
                );

                recordStmt->setInt(1, sessionId);

                std::unique_ptr<sql::ResultSet> recordResult(
                    recordStmt->executeQuery()
                );

                while (recordResult->next())
                {
                    int studentId =
                        recordResult->getInt("student_id");

                    for each (DataGridViewRow^ row in
                             this->attendanceGrid->Rows)
                    {
                        if (row->Cells["StudentId"]->Value != nullptr &&
                            Convert::ToInt32(
                                row->Cells["StudentId"]->Value
                            ) == studentId)
                        {
                            row->Cells["AttendanceStatus"]->Value =
                                gcnew String(
                                    recordResult->getString(
                                        "attendance_status"
                                    ).c_str()
                                );

                            row->Cells["Remarks"]->Value =
                                gcnew String(
                                    recordResult->getString(
                                        "remarks"
                                    ).c_str()
                                );

                            break;
                        }
                    }
                }

                this->lblSessionInfo->Text =
                    L"Existing attendance loaded. Review and save any changes.";
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }

        int GetOrCreateSession(
            int yearId,
            int termId,
            int classId,
            int streamId)
        {
            String^ dateText =
                this->dtpAttendanceDate->Value.ToString(L"yyyy-MM-dd");

            std::unique_ptr<sql::PreparedStatement> findStmt;

            if (streamId > 0)
            {
                findStmt.reset(
                    this->currentConnection->prepareStatement(
                        "SELECT attendance_session_id "
                        "FROM attendance_sessions "
                        "WHERE attendance_date = ? "
                        "AND academic_year_id = ? "
                        "AND term_id = ? "
                        "AND class_id = ? "
                        "AND stream_id = ? "
                        "LIMIT 1"
                    )
                );

                findStmt->setString(
                    1,
                    msclr::interop::marshal_as<std::string>(dateText)
                );
                findStmt->setInt(2, yearId);
                findStmt->setInt(3, termId);
                findStmt->setInt(4, classId);
                findStmt->setInt(5, streamId);
            }
            else
            {
                findStmt.reset(
                    this->currentConnection->prepareStatement(
                        "SELECT attendance_session_id "
                        "FROM attendance_sessions "
                        "WHERE attendance_date = ? "
                        "AND academic_year_id = ? "
                        "AND term_id = ? "
                        "AND class_id = ? "
                        "AND stream_id IS NULL "
                        "LIMIT 1"
                    )
                );

                findStmt->setString(
                    1,
                    msclr::interop::marshal_as<std::string>(dateText)
                );
                findStmt->setInt(2, yearId);
                findStmt->setInt(3, termId);
                findStmt->setInt(4, classId);
            }

            std::unique_ptr<sql::ResultSet> findResult(
                findStmt->executeQuery()
            );

            if (findResult->next())
                return findResult->getInt("attendance_session_id");

            std::unique_ptr<sql::PreparedStatement> insertStmt;

            if (streamId > 0)
            {
                insertStmt.reset(
                    this->currentConnection->prepareStatement(
                        "INSERT INTO attendance_sessions "
                        "(attendance_date, academic_year_id, term_id, class_id, stream_id, recorded_by) "
                        "VALUES (?, ?, ?, ?, ?, ?)"
                    )
                );

                insertStmt->setString(
                    1,
                    msclr::interop::marshal_as<std::string>(dateText)
                );
                insertStmt->setInt(2, yearId);
                insertStmt->setInt(3, termId);
                insertStmt->setInt(4, classId);
                insertStmt->setInt(5, streamId);
                insertStmt->setInt(6, AuthSession::UserId);
            }
            else
            {
                insertStmt.reset(
                    this->currentConnection->prepareStatement(
                        "INSERT INTO attendance_sessions "
                        "(attendance_date, academic_year_id, term_id, class_id, stream_id, recorded_by) "
                        "VALUES (?, ?, ?, ?, NULL, ?)"
                    )
                );

                insertStmt->setString(
                    1,
                    msclr::interop::marshal_as<std::string>(dateText)
                );
                insertStmt->setInt(2, yearId);
                insertStmt->setInt(3, termId);
                insertStmt->setInt(4, classId);
                insertStmt->setInt(5, AuthSession::UserId);
            }

            insertStmt->executeUpdate();

            std::unique_ptr<sql::Statement> idStmt(
                this->currentConnection->createStatement()
            );

            std::unique_ptr<sql::ResultSet> idResult(
                idStmt->executeQuery("SELECT LAST_INSERT_ID() AS id")
            );

            if (!idResult->next())
                throw std::runtime_error(
                    "Attendance session was created but its ID could not be retrieved."
                );

            return idResult->getInt("id");
        }

        void SaveAttendance()
        {
            if (this->attendanceGrid->Rows->Count == 0)
            {
                MessageBox::Show(
                    L"Load the students before saving attendance.",
                    L"Attendance",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                return;
            }

            FilterItem^ yearItem =
                dynamic_cast<FilterItem^>(
                    this->cmbAcademicYear->SelectedItem);
            FilterItem^ termItem =
                dynamic_cast<FilterItem^>(
                    this->cmbTerm->SelectedItem);
            FilterItem^ classItem =
                dynamic_cast<FilterItem^>(
                    this->cmbClass->SelectedItem);

            if (yearItem == nullptr ||
                termItem == nullptr ||
                classItem == nullptr)
                return;

            int streamId = 0;
            if (this->cmbStream->SelectedIndex > 0)
            {
                FilterItem^ streamItem =
                    dynamic_cast<FilterItem^>(
                        this->cmbStream->SelectedItem);

                if (streamItem != nullptr)
                    streamId = streamItem->Id;
            }

            try
            {
                auto con = DbConnection::GetConnection();
                con->setAutoCommit(false);

                // Find or create the attendance session.
                int sessionId = 0;

                String^ dateText =
                    this->dtpAttendanceDate->Value.ToString(L"yyyy-MM-dd");

                std::unique_ptr<sql::PreparedStatement> findStmt;

                if (streamId > 0)
                {
                    findStmt.reset(
                        con->prepareStatement(
                            "SELECT attendance_session_id "
                            "FROM attendance_sessions "
                            "WHERE attendance_date = ? "
                            "AND academic_year_id = ? "
                            "AND term_id = ? "
                            "AND class_id = ? "
                            "AND stream_id = ? "
                            "LIMIT 1"
                        )
                    );

                    findStmt->setString(
                        1,
                        msclr::interop::marshal_as<std::string>(dateText));
                    findStmt->setInt(2, yearItem->Id);
                    findStmt->setInt(3, termItem->Id);
                    findStmt->setInt(4, classItem->Id);
                    findStmt->setInt(5, streamId);
                }
                else
                {
                    findStmt.reset(
                        con->prepareStatement(
                            "SELECT attendance_session_id "
                            "FROM attendance_sessions "
                            "WHERE attendance_date = ? "
                            "AND academic_year_id = ? "
                            "AND term_id = ? "
                            "AND class_id = ? "
                            "AND stream_id IS NULL "
                            "LIMIT 1"
                        )
                    );

                    findStmt->setString(
                        1,
                        msclr::interop::marshal_as<std::string>(dateText));
                    findStmt->setInt(2, yearItem->Id);
                    findStmt->setInt(3, termItem->Id);
                    findStmt->setInt(4, classItem->Id);
                }

                std::unique_ptr<sql::ResultSet> findResult(
                    findStmt->executeQuery());

                if (findResult->next())
                {
                    sessionId =
                        findResult->getInt("attendance_session_id");
                }
                else
                {
                    std::unique_ptr<sql::PreparedStatement> insertSession;

                    if (streamId > 0)
                    {
                        insertSession.reset(
                            con->prepareStatement(
                                "INSERT INTO attendance_sessions "
                                "(attendance_date, academic_year_id, term_id, class_id, stream_id, recorded_by) "
                                "VALUES (?, ?, ?, ?, ?, ?)"
                            )
                        );

                        insertSession->setString(
                            1,
                            msclr::interop::marshal_as<std::string>(
                                dateText));
                        insertSession->setInt(2, yearItem->Id);
                        insertSession->setInt(3, termItem->Id);
                        insertSession->setInt(4, classItem->Id);
                        insertSession->setInt(5, streamId);
                        insertSession->setInt(6, AuthSession::UserId);
                    }
                    else
                    {
                        insertSession.reset(
                            con->prepareStatement(
                                "INSERT INTO attendance_sessions "
                                "(attendance_date, academic_year_id, term_id, class_id, stream_id, recorded_by) "
                                "VALUES (?, ?, ?, ?, NULL, ?)"
                            )
                        );

                        insertSession->setString(
                            1,
                            msclr::interop::marshal_as<std::string>(
                                dateText));
                        insertSession->setInt(2, yearItem->Id);
                        insertSession->setInt(3, termItem->Id);
                        insertSession->setInt(4, classItem->Id);
                        insertSession->setInt(5, AuthSession::UserId);
                    }

                    insertSession->executeUpdate();

                    std::unique_ptr<sql::Statement> idStmt(
                        con->createStatement());
                    std::unique_ptr<sql::ResultSet> idResult(
                        idStmt->executeQuery(
                            "SELECT LAST_INSERT_ID() AS id"));

                    if (!idResult->next())
                        throw std::runtime_error(
                            "Could not retrieve the attendance session ID.");

                    sessionId = idResult->getInt("id");
                }

                std::unique_ptr<sql::PreparedStatement> recordStmt(
                    con->prepareStatement(
                        "INSERT INTO attendance_records "
                        "(attendance_session_id, student_id, attendance_status, remarks) "
                        "VALUES (?, ?, ?, ?) "
                        "ON DUPLICATE KEY UPDATE "
                        "attendance_status = VALUES(attendance_status), "
                        "remarks = VALUES(remarks)"
                    )
                );

                for each (DataGridViewRow^ row in
                          this->attendanceGrid->Rows)
                {
                    if (row->IsNewRow)
                        continue;

                    if (row->Cells["StudentId"]->Value == nullptr)
                        continue;

                    int studentId =
                        Convert::ToInt32(
                            row->Cells["StudentId"]->Value);

                    String^ status =
                        Convert::ToString(
                            row->Cells["AttendanceStatus"]->Value);

                    String^ remarks =
                        row->Cells["Remarks"]->Value == nullptr
                            ? L""
                            : Convert::ToString(
                                row->Cells["Remarks"]->Value);

                    if (String::IsNullOrWhiteSpace(status))
                        status = L"Present";

                    recordStmt->setInt(1, sessionId);
                    recordStmt->setInt(2, studentId);
                    recordStmt->setString(
                        3,
                        msclr::interop::marshal_as<std::string>(
                            status));
                    recordStmt->setString(
                        4,
                        msclr::interop::marshal_as<std::string>(
                            remarks->Trim()));

                    recordStmt->executeUpdate();
                }

                con->commit();
                con->setAutoCommit(true);

                MessageBox::Show(
                    L"Attendance saved successfully.",
                    L"Attendance",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information
                );

                this->lblSessionInfo->Text =
                    L"Attendance saved for " +
                    this->dtpAttendanceDate->Value.ToString(L"d") +
                    L". You can review or update it.";
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
            catch (Exception^ ex)
            {
                MessageBox::Show(
                    ex->Message,
                    L"Attendance Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }

        void RefreshAttendance()
        {
            this->cmbAcademicYear->SelectedIndex = 0;
            this->cmbTerm->Items->Clear();
            this->cmbTerm->Items->Add(L"Select term");
            this->cmbTerm->SelectedIndex = 0;

            this->cmbClass->SelectedIndex = 0;
            this->cmbStream->Items->Clear();
            this->cmbStream->Items->Add(L"All streams");
            this->cmbStream->SelectedIndex = 0;

            this->dtpAttendanceDate->Value = DateTime::Today;

            ClearAttendanceGrid();

            this->lblSessionInfo->Text =
                L"Select the academic year, term and class, then load students.";
        }

        System::Void cmbAcademicYear_SelectedIndexChanged(
            System::Object^ sender,
            EventArgs^ e)
        {
            LoadTerms();
            LoadClasses();
            ClearAttendanceGrid();
        }

        System::Void cmbClass_SelectedIndexChanged(
            System::Object^ sender,
            EventArgs^ e)
        {
            LoadStreams();
            ClearAttendanceGrid();
        }

        System::Void btnLoadStudents_Click(
            System::Object^ sender,
            EventArgs^ e)
        {
            LoadStudents();
        }

        System::Void btnSaveAttendance_Click(
            System::Object^ sender,
            EventArgs^ e)
        {
            SaveAttendance();
        }

        System::Void btnRefresh_Click(
            System::Object^ sender,
            EventArgs^ e)
        {
            RefreshAttendance();
        }

        System::Void btnBack_Click(
            System::Object^ sender,
            EventArgs^ e)
        {
            if (this->Parent != nullptr)
            {
                Control^ parent = this->Parent;
                parent->Controls->Remove(this);
                this->Hide();
            }
            else
            {
                this->Close();
            }
        }

    public:
        ref class FilterItem
        {
        public:
            int Id;
            String^ Name;

            FilterItem(int id, String^ name)
            {
                Id = id;
                Name = name;
            }

            virtual String^ ToString() override
            {
                return Name;
            }
        };

        Attendance()
        {
            InitializeComponent();

            this->cmbAcademicYear->SelectedIndexChanged +=
                gcnew EventHandler(
                    this,
                    &Attendance::cmbAcademicYear_SelectedIndexChanged
                );

            this->cmbClass->SelectedIndexChanged +=
                gcnew EventHandler(
                    this,
                    &Attendance::cmbClass_SelectedIndexChanged
                );

            this->btnLoadStudents->Click +=
                gcnew EventHandler(
                    this,
                    &Attendance::btnLoadStudents_Click
                );

            this->btnSaveAttendance->Click +=
                gcnew EventHandler(
                    this,
                    &Attendance::btnSaveAttendance_Click
                );

            this->btnRefresh->Click +=
                gcnew EventHandler(
                    this,
                    &Attendance::btnRefresh_Click
                );

            this->btnBack->Click +=
                gcnew EventHandler(
                    this,
                    &Attendance::btnBack_Click
                );

            LoadAcademicYears();
        }
    };
}

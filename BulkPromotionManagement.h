#pragma once

#include "DbConnection.h"
#include "ThemeManager.h"

#include <mariadb/conncpp.hpp>
#include <msclr/marshal_cppstd.h>

#include <memory>
#include <string>
#include <stdexcept>

namespace SchoolCore
{
    using namespace System;
    using namespace System::Drawing;
    using namespace System::Windows::Forms;

    public ref class BulkPromotionManagement : public Form
    {
    public:
        BulkPromotionManagement()
        {
            this->studentIds = nullptr;

            InitializeComponent();
            ThemeManager::ApplyToForm(this);

            if (System::ComponentModel::LicenseManager::UsageMode !=
                System::ComponentModel::LicenseUsageMode::Designtime)
            {
                LoadSourceClasses();
                LoadAcademicYears();
                LoadTerms();
                LoadTargetClasses();
                LoadOptionalSubjects();
                LoadStudents();
            }
        }

        BulkPromotionManagement(array<long long>^ studentIds)
        {
            this->studentIds = studentIds;

            InitializeComponent();
            ThemeManager::ApplyToForm(this);

            if (System::ComponentModel::LicenseManager::UsageMode !=
                System::ComponentModel::LicenseUsageMode::Designtime)
            {
                LoadSourceClasses();
                LoadAcademicYears();
                LoadTerms();
                LoadTargetClasses();
                LoadOptionalSubjects();
                LoadStudents();
            }
        }

    private:
        array<long long>^ studentIds;

        TableLayoutPanel^ mainLayout;
        Label^ lblTitle;
        Label^ lblSubtitle;
        Label^ lblCount;

        ComboBox^ cmbSourceClass;
        ComboBox^ cmbAcademicYear;
        ComboBox^ cmbTerm;
        ComboBox^ cmbTargetClass;
        Button^ btnLoadClass;

        DataGridView^ studentsGrid;
        DataGridViewComboBoxColumn^ streamColumn;
        DataGridViewComboBoxColumn^ option1Column;
        DataGridViewComboBoxColumn^ option2Column;

        Button^ btnPromote;
        Button^ btnCancel;

        ref class ComboItem
        {
        public:
            int Id;
            String^ Text;

            ComboItem(int id, String^ text)
            {
                Id = id;
                Text = text;
            }

            virtual String^ ToString() override
            {
                return Text;
            }
        };

        void InitializeComponent()
        {
            this->Text = L"Bulk Student Promotion";
            this->StartPosition = FormStartPosition::CenterParent;
            this->MinimumSize = System::Drawing::Size(980, 640);
            this->ClientSize = System::Drawing::Size(1180, 760);
            this->BackColor = Color::WhiteSmoke;

            this->mainLayout = gcnew TableLayoutPanel();
            this->mainLayout->Dock = DockStyle::Fill;
            this->mainLayout->Padding = System::Windows::Forms::Padding(22);
            this->mainLayout->ColumnCount = 2;
            this->mainLayout->RowCount = 5;
            this->mainLayout->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Percent, 50.0F));
            this->mainLayout->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Percent, 50.0F));
            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 70.0F));
            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 82.0F));
            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 42.0F));
            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(SizeType::Percent, 100.0F));
            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(SizeType::Absolute, 58.0F));

            this->lblTitle = gcnew Label();
            this->lblTitle->Text = L"Bulk Promotion — Senior 2 to Senior 3";
            this->lblTitle->Dock = DockStyle::Fill;
            this->lblTitle->Font =
                gcnew Drawing::Font(L"Segoe UI Semibold", 18.0F, FontStyle::Bold);
            this->lblTitle->ForeColor = Color::FromArgb(30, 41, 59);
            this->lblTitle->TextAlign = ContentAlignment::MiddleLeft;
            this->mainLayout->Controls->Add(this->lblTitle, 0, 0);
            this->mainLayout->SetColumnSpan(this->lblTitle, 2);

            FlowLayoutPanel^ periodPanel = gcnew FlowLayoutPanel();
            periodPanel->Dock = DockStyle::Fill;
            periodPanel->WrapContents = false;
            periodPanel->Padding = System::Windows::Forms::Padding(0, 12, 0, 8);

            Label^ sourceLabel = gcnew Label();
            sourceLabel->Text = L"Current Class";
            sourceLabel->AutoSize = true;
            sourceLabel->Margin = System::Windows::Forms::Padding(0, 9, 8, 0);

            this->cmbSourceClass = gcnew ComboBox();
            this->cmbSourceClass->DropDownStyle = ComboBoxStyle::DropDownList;
            this->cmbSourceClass->Width = 150;
            this->cmbSourceClass->Margin = System::Windows::Forms::Padding(0, 4, 8, 0);

            this->btnLoadClass = gcnew Button();
            this->btnLoadClass->Text = L"Load Students";
            this->btnLoadClass->Width = 115;
            this->btnLoadClass->Height = 34;
            this->btnLoadClass->Margin = System::Windows::Forms::Padding(0, 4, 16, 0);
            this->btnLoadClass->Click +=
                gcnew EventHandler(this, &BulkPromotionManagement::btnLoadClass_Click);

            Label^ yearLabel = gcnew Label();
            yearLabel->Text = L"Academic Year";
            yearLabel->AutoSize = true;
            yearLabel->Margin = System::Windows::Forms::Padding(0, 9, 8, 0);

            this->cmbAcademicYear = gcnew ComboBox();
            this->cmbAcademicYear->DropDownStyle = ComboBoxStyle::DropDownList;
            this->cmbAcademicYear->Width = 190;
            this->cmbAcademicYear->Margin = System::Windows::Forms::Padding(0, 4, 18, 0);
            this->cmbAcademicYear->SelectedIndexChanged +=
                gcnew EventHandler(this, &BulkPromotionManagement::cmbAcademicYear_SelectedIndexChanged);

            Label^ termLabel = gcnew Label();
            termLabel->Text = L"Term";
            termLabel->AutoSize = true;
            termLabel->Margin = System::Windows::Forms::Padding(0, 9, 8, 0);

            this->cmbTerm = gcnew ComboBox();
            this->cmbTerm->DropDownStyle = ComboBoxStyle::DropDownList;
            this->cmbTerm->Width = 150;
            this->cmbTerm->Margin = System::Windows::Forms::Padding(0, 4, 18, 0);

            Label^ targetLabel = gcnew Label();
            targetLabel->Text = L"Target Class";
            targetLabel->AutoSize = true;
            targetLabel->Margin = System::Windows::Forms::Padding(0, 9, 8, 0);

            this->cmbTargetClass = gcnew ComboBox();
            this->cmbTargetClass->DropDownStyle = ComboBoxStyle::DropDownList;
            this->cmbTargetClass->Width = 160;
            this->cmbTargetClass->Margin = System::Windows::Forms::Padding(0, 4, 0, 0);
            this->cmbTargetClass->SelectedIndexChanged +=
                gcnew EventHandler(this, &BulkPromotionManagement::cmbTargetClass_SelectedIndexChanged);

            periodPanel->Controls->Add(sourceLabel);
            periodPanel->Controls->Add(this->cmbSourceClass);
            periodPanel->Controls->Add(this->btnLoadClass);
            periodPanel->Controls->Add(yearLabel);
            periodPanel->Controls->Add(this->cmbAcademicYear);
            periodPanel->Controls->Add(termLabel);
            periodPanel->Controls->Add(this->cmbTerm);
            periodPanel->Controls->Add(targetLabel);
            periodPanel->Controls->Add(this->cmbTargetClass);

            this->mainLayout->Controls->Add(periodPanel, 0, 1);
            this->mainLayout->SetColumnSpan(periodPanel, 2);

            this->lblCount = gcnew Label();
            this->lblCount->Dock = DockStyle::Fill;
            this->lblCount->Font =
                gcnew Drawing::Font(L"Segoe UI", 9.5F, FontStyle::Bold);
            this->lblCount->ForeColor = Color::FromArgb(71, 85, 105);
            this->lblCount->TextAlign = ContentAlignment::MiddleLeft;
            this->mainLayout->Controls->Add(this->lblCount, 0, 2);
            this->mainLayout->SetColumnSpan(this->lblCount, 2);

            this->studentsGrid = gcnew DataGridView();
            this->studentsGrid->Dock = DockStyle::Fill;
            this->studentsGrid->AllowUserToAddRows = false;
            this->studentsGrid->AllowUserToDeleteRows = false;
            this->studentsGrid->AllowUserToResizeRows = false;
            this->studentsGrid->AutoGenerateColumns = false;
            this->studentsGrid->BackgroundColor = Color::White;
            this->studentsGrid->BorderStyle = BorderStyle::None;
            this->studentsGrid->Font =
                gcnew Drawing::Font(L"Segoe UI", 9.0F);
            this->studentsGrid->RowHeadersVisible = false;
            this->studentsGrid->SelectionMode = DataGridViewSelectionMode::FullRowSelect;
            this->studentsGrid->MultiSelect = false;
            this->studentsGrid->ReadOnly = false;
            this->studentsGrid->RowTemplate->Height = 36;
            this->studentsGrid->ColumnHeadersHeight = 42;
            this->studentsGrid->EnableHeadersVisualStyles = false;
            this->studentsGrid->ColumnHeadersDefaultCellStyle->BackColor =
                Color::FromArgb(30, 41, 59);
            this->studentsGrid->ColumnHeadersDefaultCellStyle->ForeColor = Color::White;

            DataGridViewCheckBoxColumn^ selectColumn =
                gcnew DataGridViewCheckBoxColumn();
            selectColumn->Name = L"Select";
            selectColumn->HeaderText = L"Select";
            selectColumn->Width = 55;
            selectColumn->ReadOnly = false;
            this->studentsGrid->Columns->Add(selectColumn);

            DataGridViewTextBoxColumn^ idColumn =
                gcnew DataGridViewTextBoxColumn();
            idColumn->Name = L"StudentId";
            idColumn->Visible = false;
            idColumn->ReadOnly = true;
            this->studentsGrid->Columns->Add(idColumn);

            DataGridViewTextBoxColumn^ regColumn =
                gcnew DataGridViewTextBoxColumn();
            regColumn->Name = L"Registration";
            regColumn->HeaderText = L"Registration No.";
            regColumn->Width = 150;
            regColumn->ReadOnly = true;
            this->studentsGrid->Columns->Add(regColumn);

            DataGridViewTextBoxColumn^ nameColumn =
                gcnew DataGridViewTextBoxColumn();
            nameColumn->Name = L"StudentName";
            nameColumn->HeaderText = L"Student Name";
            nameColumn->Width = 240;
            nameColumn->ReadOnly = true;
            this->studentsGrid->Columns->Add(nameColumn);

            DataGridViewTextBoxColumn^ currentStreamColumn =
                gcnew DataGridViewTextBoxColumn();
            currentStreamColumn->Name = L"CurrentStream";
            currentStreamColumn->HeaderText = L"Current Stream";
            currentStreamColumn->Width = 115;
            currentStreamColumn->ReadOnly = true;
            this->studentsGrid->Columns->Add(currentStreamColumn);

            DataGridViewTextBoxColumn^ currentClassColumn =
                gcnew DataGridViewTextBoxColumn();
            currentClassColumn->Name = L"CurrentClass";
            currentClassColumn->HeaderText = L"Current Class";
            currentClassColumn->Width = 110;
            currentClassColumn->ReadOnly = true;
            this->studentsGrid->Columns->Add(currentClassColumn);

            this->streamColumn = gcnew DataGridViewComboBoxColumn();
            this->streamColumn->Name = L"TargetStream";
            this->streamColumn->HeaderText = L"Target Stream";
            this->streamColumn->Width = 150;
            this->streamColumn->FlatStyle = FlatStyle::Flat;
            this->studentsGrid->Columns->Add(this->streamColumn);

            this->option1Column = gcnew DataGridViewComboBoxColumn();
            this->option1Column->Name = L"Option1";
            this->option1Column->HeaderText = L"Optional Subject 1";
            this->option1Column->Width = 190;
            this->option1Column->FlatStyle = FlatStyle::Flat;
            this->studentsGrid->Columns->Add(this->option1Column);

            this->option2Column = gcnew DataGridViewComboBoxColumn();
            this->option2Column->Name = L"Option2";
            this->option2Column->HeaderText = L"Optional Subject 2";
            this->option2Column->Width = 190;
            this->option2Column->FlatStyle = FlatStyle::Flat;
            this->studentsGrid->Columns->Add(this->option2Column);

            this->mainLayout->Controls->Add(this->studentsGrid, 0, 3);
            this->mainLayout->SetColumnSpan(this->studentsGrid, 2);

            FlowLayoutPanel^ footer = gcnew FlowLayoutPanel();
            footer->Dock = DockStyle::Fill;
            footer->FlowDirection = FlowDirection::RightToLeft;
            footer->WrapContents = false;
            footer->Padding = System::Windows::Forms::Padding(0, 8, 0, 0);

            this->btnCancel = gcnew Button();
            this->btnCancel->Text = L"Cancel";
            this->btnCancel->Width = 110;
            this->btnCancel->Height = 40;
            this->btnCancel->Margin = System::Windows::Forms::Padding(8, 0, 0, 0);
            this->btnCancel->DialogResult =
                System::Windows::Forms::DialogResult::Cancel;

            this->btnPromote = gcnew Button();
            this->btnPromote->Text = L"Promote Checked Students";
            this->btnPromote->Width = 220;
            this->btnPromote->Height = 40;
            this->btnPromote->Margin = System::Windows::Forms::Padding(8, 0, 0, 0);
            this->btnPromote->BackColor = Color::FromArgb(30, 41, 59);
            this->btnPromote->ForeColor = Color::White;
            this->btnPromote->FlatStyle = FlatStyle::Flat;
            this->btnPromote->FlatAppearance->BorderSize = 0;
            this->btnPromote->Click +=
                gcnew EventHandler(this, &BulkPromotionManagement::btnPromote_Click);

            footer->Controls->Add(this->btnCancel);
            footer->Controls->Add(this->btnPromote);

            this->mainLayout->Controls->Add(footer, 0, 4);
            this->mainLayout->SetColumnSpan(footer, 2);

            this->Controls->Add(this->mainLayout);
        }

        void LoadSourceClasses()
        {
            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT c.class_id, c.class_name "
                        "FROM classes c "
                        "INNER JOIN academic_levels al "
                        "ON al.academic_level_id = c.academic_level_id "
                        "WHERE c.status = 'Active' "
                        "AND al.level_code = 'O_LEVEL' "
                        "ORDER BY c.class_id"
                    )
                );

                std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());

                this->cmbSourceClass->Items->Clear();

                while (result->next())
                {
                    this->cmbSourceClass->Items->Add(
                        gcnew ComboItem(
                            result->getInt("class_id"),
                            gcnew String(result->getString("class_name").c_str())
                        )
                    );
                }

                for (int i = 0; i < this->cmbSourceClass->Items->Count; ++i)
                {
                    ComboItem^ item =
                        safe_cast<ComboItem^>(this->cmbSourceClass->Items[i]);

                    if (item->Text->Equals(
                        L"Senior 2",
                        StringComparison::OrdinalIgnoreCase))
                    {
                        this->cmbSourceClass->SelectedIndex = i;
                        return;
                    }
                }

                if (this->cmbSourceClass->Items->Count > 0)
                {
                    this->cmbSourceClass->SelectedIndex = 0;
                }
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error);
            }
        }

        {
            try
            {
                auto con = DbConnection::GetConnection();
                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT academic_year_id, year_name "
                        "FROM academic_years "
                        "WHERE status = 'Active' "
                        "ORDER BY academic_year_id DESC"));

                std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());
                this->cmbAcademicYear->Items->Clear();

                while (result->next())
                {
                    this->cmbAcademicYear->Items->Add(
                        gcnew ComboItem(
                            result->getInt("academic_year_id"),
                            gcnew String(result->getString("year_name").c_str())));
                }

                if (this->cmbAcademicYear->Items->Count > 0)
                {
                    this->cmbAcademicYear->SelectedIndex = 0;
                }
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error);
            }
        }

        void LoadTerms()
        {
            try
            {
                auto con = DbConnection::GetConnection();
                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT term_id, term_name "
                        "FROM terms "
                        "WHERE status = 'Active' "
                        "ORDER BY term_id"));

                std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());
                this->cmbTerm->Items->Clear();

                while (result->next())
                {
                    this->cmbTerm->Items->Add(
                        gcnew ComboItem(
                            result->getInt("term_id"),
                            gcnew String(result->getString("term_name").c_str())));
                }

                if (this->cmbTerm->Items->Count > 0)
                {
                    this->cmbTerm->SelectedIndex = 0;
                }
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error);
            }
        }

        void LoadTargetClasses()
        {
            try
            {
                auto con = DbConnection::GetConnection();
                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT c.class_id, c.class_name "
                        "FROM classes c "
                        "INNER JOIN academic_levels al "
                        "ON al.academic_level_id = c.academic_level_id "
                        "WHERE c.status = 'Active' "
                        "AND al.level_code = 'O_LEVEL' "
                        "ORDER BY c.class_id"));

                std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());
                this->cmbTargetClass->Items->Clear();

                while (result->next())
                {
                    this->cmbTargetClass->Items->Add(
                        gcnew ComboItem(
                            result->getInt("class_id"),
                            gcnew String(result->getString("class_name").c_str())));
                }

                for (int i = 0; i < this->cmbTargetClass->Items->Count; ++i)
                {
                    ComboItem^ item =
                        safe_cast<ComboItem^>(this->cmbTargetClass->Items[i]);

                    if (item->Text->Equals(
                        L"Senior 3",
                        StringComparison::OrdinalIgnoreCase))
                    {
                        this->cmbTargetClass->SelectedIndex = i;
                        return;
                    }
                }

                if (this->cmbTargetClass->Items->Count > 0)
                {
                    this->cmbTargetClass->SelectedIndex = 0;
                }
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error);
            }
        }

        void LoadOptionalSubjects()
        {
            for each (DataGridViewRow^ row in this->studentsGrid->Rows)
            {
                row->Cells["Option1"]->Value = nullptr;
                row->Cells["Option2"]->Value = nullptr;
            }

            this->option1Column->Items->Clear();
            this->option2Column->Items->Clear();

            if (this->cmbAcademicYear->SelectedIndex < 0)
            {
                return;
            }

            ComboItem^ yearItem =
                safe_cast<ComboItem^>(this->cmbAcademicYear->SelectedItem);

            try
            {
                auto con = DbConnection::GetConnection();
                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT s.subject_id, s.subject_name "
                        "FROM academic_years ay "
                        "INNER JOIN curricula c "
                        "ON c.curriculum_id = ay.curriculum_id "
                        "INNER JOIN academic_levels al "
                        "ON al.academic_level_id = c.academic_level_id "
                        "INNER JOIN curriculum_subjects cs "
                        "ON cs.curriculum_id = c.curriculum_id "
                        "INNER JOIN subjects s "
                        "ON s.subject_id = cs.subject_id "
                        "WHERE ay.academic_year_id = ? "
                        "AND al.level_code = 'O_LEVEL' "
                        "AND cs.requirement_type = 'Optional' "
                        "AND cs.status = 'Active' "
                        "AND s.status = 'Active' "
                        "ORDER BY s.subject_name"));

                stmt->setInt(1, yearItem->Id);

                std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());

                while (result->next())
                {
                    ComboItem^ item = gcnew ComboItem(
                        result->getInt("subject_id"),
                        gcnew String(result->getString("subject_name").c_str()));

                    this->option1Column->Items->Add(item);
                    this->option2Column->Items->Add(
                        gcnew ComboItem(item->Id, item->Text));
                }

                SetDefaultOptionCells();
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error);
            }
        }

        void LoadStreams()
        {
            for each (DataGridViewRow^ row in this->studentsGrid->Rows)
            {
                row->Cells["TargetStream"]->Value = nullptr;
            }

            this->streamColumn->Items->Clear();

            if (this->cmbTargetClass->SelectedIndex < 0)
            {
                return;
            }

            ComboItem^ classItem =
                safe_cast<ComboItem^>(this->cmbTargetClass->SelectedItem);

            try
            {
                auto con = DbConnection::GetConnection();
                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT stream_id, stream_name "
                        "FROM streams "
                        "WHERE class_id = ? "
                        "AND status = 'Active' "
                        "ORDER BY stream_id"));

                stmt->setInt(1, classItem->Id);

                std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());

                while (result->next())
                {
                    this->streamColumn->Items->Add(
                        gcnew ComboItem(
                            result->getInt("stream_id"),
                            gcnew String(result->getString("stream_name").c_str())));
                }

                SetDefaultStreamCells();
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error);
            }
        }

        void LoadStudents()
        {
            this->studentsGrid->Rows->Clear();

            ComboItem^ sourceItem =
                this->cmbSourceClass->SelectedIndex >= 0
                    ? safe_cast<ComboItem^>(this->cmbSourceClass->SelectedItem)
                    : nullptr;

            if (sourceItem == nullptr)
            {
                this->lblCount->Text = L"Select a current class to load students.";
                return;
            }

            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT "
                        "s.student_id, "
                        "s.registration_number, "
                        "CONCAT_WS(' ', s.first_name, s.middle_name, s.last_name) AS full_name, "
                        "c.class_name, "
                        "st.stream_name "
                        "FROM students s "
                        "INNER JOIN enrollments e "
                        "ON e.enrollment_id = ("
                            "SELECT e2.enrollment_id "
                            "FROM enrollments e2 "
                            "WHERE e2.student_id = s.student_id "
                            "AND e2.status = 'Active' "
                            "ORDER BY e2.enrollment_date DESC, e2.enrollment_id DESC "
                            "LIMIT 1"
                        ") "
                        "INNER JOIN classes c ON c.class_id = e.class_id "
                        "LEFT JOIN streams st ON st.stream_id = e.stream_id "
                        "WHERE e.class_id = ? "
                        "AND e.status = 'Active' "
                        "ORDER BY s.last_name, s.first_name, s.student_id"
                    )
                );

                stmt->setInt(1, sourceItem->Id);

                std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());

                while (result->next())
                {
                    bool preselected = false;

                    if (this->studentIds != nullptr)
                    {
                        long long currentId =
                            result->getInt64("student_id");

                        for each (long long selectedId in this->studentIds)
                        {
                            if (selectedId == currentId)
                            {
                                preselected = true;
                                break;
                            }
                        }
                    }

                    this->studentsGrid->Rows->Add(
                        preselected,
                        result->getInt64("student_id"),
                        gcnew String(result->getString("registration_number").c_str()),
                        gcnew String(result->getString("full_name").c_str()),
                        result->isNull("class_name")
                            ? L"Not assigned"
                            : gcnew String(result->getString("class_name").c_str()),
                        result->isNull("stream_name")
                            ? L"Not assigned"
                            : gcnew String(result->getString("stream_name").c_str())
                    );
                }

                this->lblCount->Text =
                    L"Students in " +
                    sourceItem->Text +
                    L": " +
                    this->studentsGrid->Rows->Count.ToString() +
                    L"    |    Tick the students to promote";

                if (this->studentsGrid->Rows->Count == 0)
                {
                    this->lblCount->Text =
                        L"No active students were found in " +
                        sourceItem->Text +
                        L".";
                }
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error);
            }

            SetDefaultStreamCells();
            SetDefaultOptionCells();
        }

        void SetDefaultStreamCells()
        {
            if (this->streamColumn->Items->Count == 0)
            {
                return;
            }

            ComboItem^ first =
                safe_cast<ComboItem^>(this->streamColumn->Items[0]);

            for each (DataGridViewRow^ row in this->studentsGrid->Rows)
            {
                if (row->Cells["TargetStream"]->Value == nullptr)
                {
                    row->Cells["TargetStream"]->Value = first;
                }
            }
        }

        void SetDefaultOptionCells()
        {
            if (this->option1Column->Items->Count == 0)
            {
                return;
            }

            ComboItem^ first =
                safe_cast<ComboItem^>(this->option1Column->Items[0]);

            ComboItem^ second =
                this->option2Column->Items->Count > 1
                    ? safe_cast<ComboItem^>(this->option2Column->Items[1])
                    : nullptr;

            for each (DataGridViewRow^ row in this->studentsGrid->Rows)
            {
                if (row->Cells["Option1"]->Value == nullptr)
                {
                    row->Cells["Option1"]->Value = first;
                }

                if (row->Cells["Option2"]->Value == nullptr)
                {
                    row->Cells["Option2"]->Value = second;
                }
            }
        }

        System::Void btnLoadClass_Click(
            Object^ sender,
            EventArgs^ e)
        {
            this->studentIds = nullptr;
            LoadStudents();
        }

        System::Void cmbAcademicYear_SelectedIndexChanged(
            Object^ sender,
            EventArgs^ e)
        {
            LoadOptionalSubjects();
        }

        System::Void cmbTargetClass_SelectedIndexChanged(
            Object^ sender,
            EventArgs^ e)
        {
            LoadStreams();
        }

        System::Void btnPromote_Click(
            Object^ sender,
            EventArgs^ e)
        {
            int selectedCount = 0;

            for each (DataGridViewRow^ row in this->studentsGrid->Rows)
            {
                if (row->Cells["Select"]->Value != nullptr &&
                    Convert::ToBoolean(row->Cells["Select"]->Value))
                {
                    selectedCount++;
                }
            }

            if (selectedCount == 0)
            {
                MessageBox::Show(
                    L"Tick at least one student to promote.",
                    L"Bulk Promotion",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning);
                return;
            }

            ComboItem^ yearItem =
                this->cmbAcademicYear->SelectedIndex >= 0
                    ? safe_cast<ComboItem^>(this->cmbAcademicYear->SelectedItem)
                    : nullptr;

            ComboItem^ termItem =
                this->cmbTerm->SelectedIndex >= 0
                    ? safe_cast<ComboItem^>(this->cmbTerm->SelectedItem)
                    : nullptr;

            ComboItem^ classItem =
                this->cmbTargetClass->SelectedIndex >= 0
                    ? safe_cast<ComboItem^>(this->cmbTargetClass->SelectedItem)
                    : nullptr;

            if (yearItem == nullptr ||
                termItem == nullptr ||
                classItem == nullptr)
            {
                MessageBox::Show(
                    L"Select the target academic year, term and class.",
                    L"Bulk Promotion",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning);
                return;
            }

            ComboItem^ sourceItem =
                this->cmbSourceClass->SelectedIndex >= 0
                    ? safe_cast<ComboItem^>(this->cmbSourceClass->SelectedItem)
                    : nullptr;

            if (sourceItem == nullptr ||
                !sourceItem->Text->Equals(
                    L"Senior 2",
                    StringComparison::OrdinalIgnoreCase))
            {
                MessageBox::Show(
                    L"Bulk promotion currently handles Senior 2 to Senior 3 only. Select Senior 2 as the current class.",
                    L"Bulk Promotion",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information);
                return;
            }

            if (!classItem->Text->Equals(
                    L"Senior 3",
                    StringComparison::OrdinalIgnoreCase))
            {
                MessageBox::Show(
                    L"Bulk promotion currently handles the S2 to S3 transition only.",
                    L"Bulk Promotion",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information);
                return;
            }

            for each (DataGridViewRow^ row in this->studentsGrid->Rows)
            {
                if (row->Cells["Select"]->Value == nullptr ||
                    !Convert::ToBoolean(row->Cells["Select"]->Value))
                {
                    continue;
                }

                ComboItem^ streamItem =
                    row->Cells["TargetStream"]->Value == nullptr
                        ? nullptr
                        : dynamic_cast<ComboItem^>(
                            row->Cells["TargetStream"]->Value);

                ComboItem^ option1 =
                    row->Cells["Option1"]->Value == nullptr
                        ? nullptr
                        : dynamic_cast<ComboItem^>(
                            row->Cells["Option1"]->Value);

                ComboItem^ option2 =
                    row->Cells["Option2"]->Value == nullptr
                        ? nullptr
                        : dynamic_cast<ComboItem^>(
                            row->Cells["Option2"]->Value);

                if (streamItem == nullptr ||
                    option1 == nullptr ||
                    option2 == nullptr)
                {
                    MessageBox::Show(
                        L"Every student must have a target stream and two optional subjects.",
                        L"Bulk Promotion",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Warning);
                    return;
                }

                if (option1->Id == option2->Id)
                {
                    MessageBox::Show(
                        L"The two optional subjects must be different for every student.",
                        L"Bulk Promotion",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Warning);
                    return;
                }
            }

            String^ confirmation =
                L"You are about to promote " +
                selectedCount.ToString() +
                L" student(s) from Senior 2 to Senior 3.\n\n" +
                L"Each student's selected stream and two optional subjects will be recorded.\n\n" +
                L"Continue?";

            if (MessageBox::Show(
                    confirmation,
                    L"Confirm Bulk Promotion",
                    MessageBoxButtons::YesNo,
                    MessageBoxIcon::Question) !=
                System::Windows::Forms::DialogResult::Yes)
            {
                return;
            }

            auto con = DbConnection::GetConnection();

            try
            {
                con->setAutoCommit(false);

                for each (DataGridViewRow^ row in this->studentsGrid->Rows)
                {
                    if (row->Cells["Select"]->Value == nullptr ||
                        !Convert::ToBoolean(row->Cells["Select"]->Value))
                    {
                        continue;
                    }

                    long long studentId =
                        Convert::ToInt64(row->Cells["StudentId"]->Value);

                    ComboItem^ streamItem =
                        safe_cast<ComboItem^>(
                            row->Cells["TargetStream"]->Value);

                    ComboItem^ option1 =
                        safe_cast<ComboItem^>(
                            row->Cells["Option1"]->Value);

                    ComboItem^ option2 =
                        safe_cast<ComboItem^>(
                            row->Cells["Option2"]->Value);

                    int currentEnrollmentId = 0;
                    int currentClassId = 0;
                    String^ currentClassName = L"";

                    {
                        std::unique_ptr<sql::PreparedStatement> stmt(
                            con->prepareStatement(
                                "SELECT e.enrollment_id, e.class_id, c.class_name "
                                "FROM enrollments e "
                                "INNER JOIN classes c ON c.class_id = e.class_id "
                                "WHERE e.student_id = ? "
                                "AND e.status = 'Active' "
                                "ORDER BY e.enrollment_date DESC, e.enrollment_id DESC "
                                "LIMIT 1"));

                        stmt->setInt64(1, studentId);

                        std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());

                        if (!result->next())
                        {
                            throw std::runtime_error(
                                "One of the selected students has no active enrollment.");
                        }

                        currentEnrollmentId = result->getInt("enrollment_id");
                        currentClassId = result->getInt("class_id");
                        currentClassName =
                            gcnew String(result->getString("class_name").c_str());
                    }

                    if (!currentClassName->Equals(
                            L"Senior 2",
                            StringComparison::OrdinalIgnoreCase))
                    {
                        throw std::runtime_error(
                            "All selected students must currently be in Senior 2.");
                    }

                    {
                        std::unique_ptr<sql::PreparedStatement> check(
                            con->prepareStatement(
                                "SELECT enrollment_id "
                                "FROM enrollments "
                                "WHERE student_id = ? "
                                "AND academic_year_id = ? "
                                "AND term_id = ? "
                                "LIMIT 1"));

                        check->setInt64(1, studentId);
                        check->setInt(2, yearItem->Id);
                        check->setInt(3, termItem->Id);

                        std::unique_ptr<sql::ResultSet> result(check->executeQuery());

                        if (result->next())
                        {
                            throw std::runtime_error(
                                "One of the selected students already has an enrollment in the selected academic year and term.");
                        }
                    }

                    {
                        std::unique_ptr<sql::PreparedStatement> insertEnrollment(
                            con->prepareStatement(
                                "INSERT INTO enrollments "
                                "(student_id, academic_year_id, term_id, class_id, stream_id, combination_id, enrollment_date, status) "
                                "VALUES (?, ?, ?, ?, ?, NULL, ?, 'Active')"));

                        insertEnrollment->setInt64(1, studentId);
                        insertEnrollment->setInt(2, yearItem->Id);
                        insertEnrollment->setInt(3, termItem->Id);
                        insertEnrollment->setInt(4, classItem->Id);
                        insertEnrollment->setInt(5, streamItem->Id);
                        insertEnrollment->setString(
                            6,
                            msclr::interop::marshal_as<std::string>(
                                DateTime::Today.ToString(L"yyyy-MM-dd")));

                        insertEnrollment->executeUpdate();
                    }

                    int newEnrollmentId = 0;

                    {
                        std::unique_ptr<sql::Statement> keyStmt(
                            con->createStatement());
                        std::unique_ptr<sql::ResultSet> keys(
                            keyStmt->executeQuery(
                                "SELECT LAST_INSERT_ID() AS enrollment_id"));

                        if (!keys->next())
                        {
                            throw std::runtime_error(
                                "Could not obtain a new enrollment ID.");
                        }

                        newEnrollmentId = keys->getInt("enrollment_id");
                    }

                    {
                        std::unique_ptr<sql::PreparedStatement> insertOption(
                            con->prepareStatement(
                                "INSERT INTO enrollment_optional_subjects "
                                "(enrollment_id, subject_id, option_number, selected_date, status) "
                                "VALUES (?, ?, ?, ?, 'Active')"));

                        insertOption->setInt(1, newEnrollmentId);
                        insertOption->setInt(2, option1->Id);
                        insertOption->setInt(3, 1);
                        insertOption->setString(
                            4,
                            msclr::interop::marshal_as<std::string>(
                                DateTime::Today.ToString(L"yyyy-MM-dd")));
                        insertOption->executeUpdate();

                        insertOption->setInt(1, newEnrollmentId);
                        insertOption->setInt(2, option2->Id);
                        insertOption->setInt(3, 2);
                        insertOption->setString(
                            4,
                            msclr::interop::marshal_as<std::string>(
                                DateTime::Today.ToString(L"yyyy-MM-dd")));
                        insertOption->executeUpdate();
                    }

                    {
                        std::unique_ptr<sql::PreparedStatement> closeOld(
                            con->prepareStatement(
                                "UPDATE enrollments "
                                "SET status = 'Completed' "
                                "WHERE enrollment_id = ? "
                                "AND student_id = ?"));

                        closeOld->setInt(1, currentEnrollmentId);
                        closeOld->setInt64(2, studentId);
                        closeOld->executeUpdate();
                    }
                }

                con->commit();
                con->setAutoCommit(true);

                MessageBox::Show(
                    L"All selected students have been promoted successfully from Senior 2 to Senior 3.",
                    L"Promotion Complete",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information);

                this->DialogResult =
                    System::Windows::Forms::DialogResult::OK;
                this->Close();
            }
            catch (sql::SQLException& ex)
            {
                try
                {
                    con->rollback();
                    con->setAutoCommit(true);
                }
                catch (...)
                {
                }

                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error);
            }
            catch (std::exception& ex)
            {
                try
                {
                    con->rollback();
                    con->setAutoCommit(true);
                }
                catch (...)
                {
                }

                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Bulk Promotion",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error);
            }
        }
    };
}

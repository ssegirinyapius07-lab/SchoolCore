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
            LoadInitialData();
        }

        BulkPromotionManagement(array<long long>^ studentIds)
        {
            this->studentIds = studentIds;
            InitializeComponent();
            ThemeManager::ApplyToForm(this);
            LoadInitialData();
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
        DataGridViewComboBoxColumn^ combinationColumn;

        Button^ btnPromote;
        Button^ btnCancel;

        ref class ComboItem
        {
        public:
            int Id;
            String^ Text;
            String^ LevelCode;
            int Grade;

            ComboItem(
                int id,
                String^ text,
                String^ levelCode,
                int grade)
            {
                Id = id;
                Text = text;
                LevelCode = levelCode;
                Grade = grade;
            }

            virtual String^ ToString() override
            {
                return Text;
            }
        };

        void LoadInitialData()
        {
            if (System::ComponentModel::LicenseManager::UsageMode ==
                System::ComponentModel::LicenseUsageMode::Designtime)
            {
                return;
            }

            try
            {
                LoadSourceClasses();
                LoadAcademicYears();
                LoadTerms();

                if (this->cmbSourceClass->SelectedIndex >= 0)
                {
                    LoadTargetClasses();
                }
            }
            catch (System::Exception^ ex)
            {
                MessageBox::Show(
                    ex->Message,
                    L"Promotion",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error);
            }
        }

        int GetGradeNumber(String^ className)
        {
            if (String::IsNullOrWhiteSpace(className))
            {
                return 0;
            }

            String^ value = className->Trim();

            if (!value->StartsWith(
                    L"Senior ",
                    StringComparison::OrdinalIgnoreCase))
            {
                return 0;
            }

            try
            {
                return Convert::ToInt32(value->Substring(7));
            }
            catch (System::Exception^)
            {
                return 0;
            }
        }

        ComboItem^ GetSelectedClassItem(ComboBox^ combo)
        {
            if (combo == nullptr || combo->SelectedIndex < 0)
            {
                return nullptr;
            }

            return dynamic_cast<ComboItem^>(combo->SelectedItem);
        }

        ComboItem^ FindItemById(
            DataGridViewComboBoxColumn^ column,
            int id)
        {
            if (column == nullptr)
            {
                return nullptr;
            }

            for (int i = 0; i < column->Items->Count; ++i)
            {
                ComboItem^ item =
                    dynamic_cast<ComboItem^>(column->Items[i]);

                if (item != nullptr && item->Id == id)
                {
                    return item;
                }
            }

            return nullptr;
        }

        bool IsOLevelTarget()
        {
            ComboItem^ target =
                GetSelectedClassItem(this->cmbTargetClass);

            return target != nullptr &&
                   target->LevelCode->Equals(
                       L"O_LEVEL",
                       StringComparison::OrdinalIgnoreCase);
        }

        bool IsALevelTarget()
        {
            ComboItem^ target =
                GetSelectedClassItem(this->cmbTargetClass);

            return target != nullptr &&
                   target->LevelCode->Equals(
                       L"A_LEVEL",
                       StringComparison::OrdinalIgnoreCase);
        }

        bool TargetRequiresNewOptions()
        {
            ComboItem^ target =
                GetSelectedClassItem(this->cmbTargetClass);

            return target != nullptr &&
                   target->LevelCode->Equals(
                       L"O_LEVEL",
                       StringComparison::OrdinalIgnoreCase) &&
                   target->Grade == 3;
        }

        bool TargetCarriesOLevelOptions()
        {
            ComboItem^ source =
                GetSelectedClassItem(this->cmbSourceClass);

            ComboItem^ target =
                GetSelectedClassItem(this->cmbTargetClass);

            return source != nullptr &&
                   target != nullptr &&
                   target->LevelCode->Equals(
                       L"O_LEVEL",
                       StringComparison::OrdinalIgnoreCase) &&
                   target->Grade == 4 &&
                   source->Grade == 3;
        }

        bool TargetRequiresNewCombination()
        {
            ComboItem^ target =
                GetSelectedClassItem(this->cmbTargetClass);

            return target != nullptr &&
                   target->LevelCode->Equals(
                       L"A_LEVEL",
                       StringComparison::OrdinalIgnoreCase) &&
                   target->Grade == 5;
        }

        bool TargetCarriesCombination()
        {
            ComboItem^ source =
                GetSelectedClassItem(this->cmbSourceClass);

            ComboItem^ target =
                GetSelectedClassItem(this->cmbTargetClass);

            return source != nullptr &&
                   target != nullptr &&
                   target->LevelCode->Equals(
                       L"A_LEVEL",
                       StringComparison::OrdinalIgnoreCase) &&
                   target->Grade == 6 &&
                   source->Grade == 5;
        }

        void InitializeComponent()
        {
            this->Text = L"Bulk Student Promotion";
            this->StartPosition = FormStartPosition::CenterScreen;
            this->FormBorderStyle =
                System::Windows::Forms::FormBorderStyle::Sizable;
            this->MaximizeBox = true;
            this->MinimizeBox = true;
            this->WindowState =
                System::Windows::Forms::FormWindowState::Maximized;
            this->MinimumSize = System::Drawing::Size(1050, 650);
            this->ClientSize = System::Drawing::Size(1320, 780);
            this->BackColor = Color::FromArgb(248, 250, 252);

            this->mainLayout = gcnew TableLayoutPanel();
            this->mainLayout->Dock = DockStyle::Fill;
            this->mainLayout->Padding =
                System::Windows::Forms::Padding(24);
            this->mainLayout->ColumnCount = 1;
            this->mainLayout->RowCount = 5;

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    78.0F));

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    96.0F));

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    40.0F));

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Percent,
                    100.0F));

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    58.0F));

            // Header
            Panel^ headerPanel = gcnew Panel();
            headerPanel->Dock = DockStyle::Fill;
            headerPanel->BackColor =
                Color::FromArgb(30, 41, 59);
            headerPanel->Padding =
                System::Windows::Forms::Padding(
                    18, 8, 18, 8);

            this->lblTitle = gcnew Label();
            this->lblTitle->Text = L"Bulk Student Promotion";
            this->lblTitle->Dock = DockStyle::Top;
            this->lblTitle->Height = 38;
            this->lblTitle->Font =
                gcnew Drawing::Font(
                    L"Segoe UI Semibold",
                    18.0F,
                    FontStyle::Bold);
            this->lblTitle->ForeColor = Color::White;
            this->lblTitle->TextAlign =
                ContentAlignment::MiddleLeft;

            this->lblSubtitle = gcnew Label();
            this->lblSubtitle->Text =
                L"Load a current class, select students, then promote them to the next class.";
            this->lblSubtitle->Dock = DockStyle::Fill;
            this->lblSubtitle->Font =
                gcnew Drawing::Font(
                    L"Segoe UI",
                    9.5F,
                    FontStyle::Regular);
            this->lblSubtitle->ForeColor =
                Color::Gainsboro;
            this->lblSubtitle->TextAlign =
                ContentAlignment::MiddleLeft;

            headerPanel->Controls->Add(
                this->lblSubtitle);
            headerPanel->Controls->Add(
                this->lblTitle);

            this->mainLayout->Controls->Add(
                headerPanel,
                0,
                0);

            // Filters
            TableLayoutPanel^ filterLayout =
                gcnew TableLayoutPanel();

            filterLayout->Dock = DockStyle::Fill;
            filterLayout->ColumnCount = 8;
            filterLayout->RowCount = 2;
            filterLayout->Padding =
                System::Windows::Forms::Padding(
                    0, 8, 0, 8);

            filterLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Absolute,
                    105.0F));
            filterLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    18.0F));
            filterLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Absolute,
                    100.0F));
            filterLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    18.0F));
            filterLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Absolute,
                    55.0F));
            filterLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    15.0F));
            filterLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Absolute,
                    105.0F));
            filterLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    21.0F));

            filterLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    40.0F));
            filterLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    40.0F));

            Label^ currentLabel =
                gcnew Label();
            currentLabel->Text = L"Current Class";
            currentLabel->Dock = DockStyle::Fill;
            currentLabel->TextAlign =
                ContentAlignment::MiddleLeft;

            this->cmbSourceClass =
                gcnew ComboBox();
            this->cmbSourceClass->Dock =
                DockStyle::Fill;
            this->cmbSourceClass->DropDownStyle =
                ComboBoxStyle::DropDownList;
            this->cmbSourceClass->SelectedIndexChanged +=
                gcnew EventHandler(
                    this,
                    &BulkPromotionManagement::cmbSourceClass_SelectedIndexChanged);

            this->btnLoadClass =
                gcnew Button();
            this->btnLoadClass->Text =
                L"Load";
            this->btnLoadClass->Dock =
                DockStyle::Fill;
            this->btnLoadClass->Margin =
                System::Windows::Forms::Padding(
                    6, 3, 6, 3);
            this->btnLoadClass->Click +=
                gcnew EventHandler(
                    this,
                    &BulkPromotionManagement::btnLoadClass_Click);

            Label^ targetLabel =
                gcnew Label();
            targetLabel->Text = L"Target Class";
            targetLabel->Dock = DockStyle::Fill;
            targetLabel->TextAlign =
                ContentAlignment::MiddleLeft;

            this->cmbTargetClass =
                gcnew ComboBox();
            this->cmbTargetClass->Dock =
                DockStyle::Fill;
            this->cmbTargetClass->DropDownStyle =
                ComboBoxStyle::DropDownList;
            this->cmbTargetClass->SelectedIndexChanged +=
                gcnew EventHandler(
                    this,
                    &BulkPromotionManagement::cmbTargetClass_SelectedIndexChanged);

            Label^ yearLabel =
                gcnew Label();
            yearLabel->Text =
                L"Academic Year";
            yearLabel->Dock =
                DockStyle::Fill;
            yearLabel->TextAlign =
                ContentAlignment::MiddleLeft;

            this->cmbAcademicYear =
                gcnew ComboBox();
            this->cmbAcademicYear->Dock =
                DockStyle::Fill;
            this->cmbAcademicYear->DropDownStyle =
                ComboBoxStyle::DropDownList;
            this->cmbAcademicYear->SelectedIndexChanged +=
                gcnew EventHandler(
                    this,
                    &BulkPromotionManagement::cmbAcademicYear_SelectedIndexChanged);

            Label^ termLabel =
                gcnew Label();
            termLabel->Text = L"Term";
            termLabel->Dock = DockStyle::Fill;
            termLabel->TextAlign =
                ContentAlignment::MiddleLeft;

            this->cmbTerm =
                gcnew ComboBox();
            this->cmbTerm->Dock =
                DockStyle::Fill;
            this->cmbTerm->DropDownStyle =
                ComboBoxStyle::DropDownList;

            filterLayout->Controls->Add(
                currentLabel, 0, 0);
            filterLayout->Controls->Add(
                this->cmbSourceClass, 1, 0);
            filterLayout->Controls->Add(
                targetLabel, 2, 0);
            filterLayout->Controls->Add(
                this->cmbTargetClass, 3, 0);
            filterLayout->Controls->Add(
                yearLabel, 4, 0);
            filterLayout->Controls->Add(
                this->cmbAcademicYear, 5, 0);
            filterLayout->Controls->Add(
                termLabel, 6, 0);
            filterLayout->Controls->Add(
                this->cmbTerm, 7, 0);

            Label^ instruction =
                gcnew Label();
            instruction->Text =
                L"Students are loaded from the current class. Target streams come from the target class; O-Level options and A-Level combinations appear only when applicable.";
            instruction->Dock =
                DockStyle::Fill;
            instruction->ForeColor =
                Color::FromArgb(71, 85, 105);
            instruction->TextAlign =
                ContentAlignment::MiddleLeft;

            filterLayout->Controls->Add(
                instruction,
                0,
                1);
            filterLayout->SetColumnSpan(
                instruction,
                6);

            filterLayout->Controls->Add(
                this->btnLoadClass,
                6,
                1);
            filterLayout->SetColumnSpan(
                this->btnLoadClass,
                2);

            this->mainLayout->Controls->Add(
                filterLayout,
                0,
                1);

            // Count / transition label
            this->lblCount = gcnew Label();
            this->lblCount->Dock =
                DockStyle::Fill;
            this->lblCount->Font =
                gcnew Drawing::Font(
                    L"Segoe UI Semibold",
                    9.5F,
                    FontStyle::Bold);
            this->lblCount->ForeColor =
                Color::FromArgb(71, 85, 105);
            this->lblCount->TextAlign =
                ContentAlignment::MiddleLeft;

            this->mainLayout->Controls->Add(
                this->lblCount,
                0,
                2);

            // Students grid
            this->studentsGrid =
                gcnew DataGridView();

            this->studentsGrid->Dock =
                DockStyle::Fill;
            this->studentsGrid->AllowUserToAddRows =
                false;
            this->studentsGrid->AllowUserToDeleteRows =
                false;
            this->studentsGrid->AllowUserToResizeRows =
                false;
            this->studentsGrid->AutoGenerateColumns =
                false;
            this->studentsGrid->BackgroundColor =
                Color::White;
            this->studentsGrid->BorderStyle =
                BorderStyle::None;
            this->studentsGrid->Font =
                gcnew Drawing::Font(
                    L"Segoe UI",
                    9.0F);
            this->studentsGrid->RowHeadersVisible =
                false;
            this->studentsGrid->SelectionMode =
                DataGridViewSelectionMode::FullRowSelect;
            this->studentsGrid->MultiSelect = false;
            this->studentsGrid->ReadOnly = false;
            this->studentsGrid->ColumnHeadersHeight = 42;
            this->studentsGrid->RowTemplate->Height = 36;
            this->studentsGrid->EnableHeadersVisualStyles = false;

            this->studentsGrid->ColumnHeadersDefaultCellStyle->BackColor =
                Color::FromArgb(30, 41, 59);
            this->studentsGrid->ColumnHeadersDefaultCellStyle->ForeColor =
                Color::White;

            this->studentsGrid->CurrentCellDirtyStateChanged +=
                gcnew EventHandler(
                    this,
                    &BulkPromotionManagement::studentsGrid_CurrentCellDirtyStateChanged);

            this->studentsGrid->CellValueChanged +=
                gcnew DataGridViewCellEventHandler(
                    this,
                    &BulkPromotionManagement::studentsGrid_CellValueChanged);

            DataGridViewCheckBoxColumn^ selectColumn =
                gcnew DataGridViewCheckBoxColumn();
            selectColumn->Name = L"Select";
            selectColumn->HeaderText = L"Select";
            selectColumn->Width = 55;
            selectColumn->ReadOnly = false;
            this->studentsGrid->Columns->Add(
                selectColumn);

            DataGridViewTextBoxColumn^ enrollmentColumn =
                gcnew DataGridViewTextBoxColumn();
            enrollmentColumn->Name =
                L"EnrollmentId";
            enrollmentColumn->Visible = false;
            enrollmentColumn->ReadOnly = true;
            this->studentsGrid->Columns->Add(
                enrollmentColumn);

            DataGridViewTextBoxColumn^ studentIdColumn =
                gcnew DataGridViewTextBoxColumn();
            studentIdColumn->Name =
                L"StudentId";
            studentIdColumn->Visible = false;
            studentIdColumn->ReadOnly = true;
            this->studentsGrid->Columns->Add(
                studentIdColumn);

            AddReadOnlyTextColumn(
                L"Registration",
                L"Registration No.",
                150);

            AddReadOnlyTextColumn(
                L"StudentName",
                L"Student Name",
                250);

            AddReadOnlyTextColumn(
                L"CurrentClass",
                L"Current Class",
                115);

            AddReadOnlyTextColumn(
                L"CurrentStream",
                L"Current Stream",
                115);

            this->streamColumn =
                gcnew DataGridViewComboBoxColumn();
            this->streamColumn->Name =
                L"TargetStream";
            this->streamColumn->HeaderText =
                L"Target Stream";
            this->streamColumn->Width = 140;
            this->streamColumn->FlatStyle =
                FlatStyle::Flat;
            this->studentsGrid->Columns->Add(
                this->streamColumn);

            this->option1Column =
                gcnew DataGridViewComboBoxColumn();
            this->option1Column->Name =
                L"Option1";
            this->option1Column->HeaderText =
                L"Optional Subject 1";
            this->option1Column->Width = 180;
            this->option1Column->FlatStyle =
                FlatStyle::Flat;
            this->studentsGrid->Columns->Add(
                this->option1Column);

            this->option2Column =
                gcnew DataGridViewComboBoxColumn();
            this->option2Column->Name =
                L"Option2";
            this->option2Column->HeaderText =
                L"Optional Subject 2";
            this->option2Column->Width = 180;
            this->option2Column->FlatStyle =
                FlatStyle::Flat;
            this->studentsGrid->Columns->Add(
                this->option2Column);

            this->combinationColumn =
                gcnew DataGridViewComboBoxColumn();
            this->combinationColumn->Name =
                L"Combination";
            this->combinationColumn->HeaderText =
                L"A-Level Combination";
            this->combinationColumn->Width = 220;
            this->combinationColumn->FlatStyle =
                FlatStyle::Flat;
            this->studentsGrid->Columns->Add(
                this->combinationColumn);

            this->mainLayout->Controls->Add(
                this->studentsGrid,
                0,
                3);

            // Footer
            FlowLayoutPanel^ footer =
                gcnew FlowLayoutPanel();

            footer->Dock =
                DockStyle::Fill;
            footer->FlowDirection =
                FlowDirection::RightToLeft;
            footer->WrapContents = false;
            footer->Padding =
                System::Windows::Forms::Padding(
                    0, 8, 0, 0);

            this->btnCancel =
                gcnew Button();
            this->btnCancel->Text = L"Cancel";
            this->btnCancel->Width = 110;
            this->btnCancel->Height = 40;
            this->btnCancel->Margin =
                System::Windows::Forms::Padding(
                    8, 0, 0, 0);
            this->btnCancel->DialogResult =
                System::Windows::Forms::DialogResult::Cancel;

            this->btnPromote =
                gcnew Button();
            this->btnPromote->Text =
                L"Promote Selected";
            this->btnPromote->Width = 190;
            this->btnPromote->Height = 40;
            this->btnPromote->Margin =
                System::Windows::Forms::Padding(
                    8, 0, 0, 0);
            this->btnPromote->BackColor =
                Color::FromArgb(30, 41, 59);
            this->btnPromote->ForeColor =
                Color::White;
            this->btnPromote->FlatStyle =
                FlatStyle::Flat;
            this->btnPromote->FlatAppearance->BorderSize =
                0;
            this->btnPromote->Click +=
                gcnew EventHandler(
                    this,
                    &BulkPromotionManagement::btnPromote_Click);

            footer->Controls->Add(
                this->btnCancel);
            footer->Controls->Add(
                this->btnPromote);

            this->mainLayout->Controls->Add(
                footer,
                0,
                4);

            this->Controls->Add(
                this->mainLayout);
        }

        void AddReadOnlyTextColumn(
            String^ name,
            String^ header,
            int width)
        {
            DataGridViewTextBoxColumn^ column =
                gcnew DataGridViewTextBoxColumn();

            column->Name = name;
            column->HeaderText = header;
            column->Width = width;
            column->ReadOnly = true;

            this->studentsGrid->Columns->Add(
                column);
        }

        void LoadSourceClasses()
        {
            try
            {
                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT "
                        "c.class_id, "
                        "c.class_name, "
                        "al.level_code "
                        "FROM classes c "
                        "INNER JOIN academic_levels al "
                        "ON al.academic_level_id = c.academic_level_id "
                        "WHERE c.status = 'Active' "
                        "AND al.status = 'Active' "
                        "ORDER BY c.class_id"
                    )
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery());

                this->cmbSourceClass->Items->Clear();

                while (result->next())
                {
                    String^ className =
                        gcnew String(
                            result->getString(
                                "class_name").c_str());

                    this->cmbSourceClass->Items->Add(
                        gcnew ComboItem(
                            result->getInt("class_id"),
                            className,
                            gcnew String(
                                result->getString(
                                    "level_code").c_str()),
                            GetGradeNumber(className)
                        )
                    );
                }

                int preferredIndex = -1;

                for (int i = 0;
                     i < this->cmbSourceClass->Items->Count;
                     ++i)
                {
                    ComboItem^ item =
                        safe_cast<ComboItem^>(
                            this->cmbSourceClass->Items[i]);

                    if (item->Text->Equals(
                            L"Senior 2",
                            StringComparison::OrdinalIgnoreCase))
                    {
                        preferredIndex = i;
                        break;
                    }
                }

                if (preferredIndex >= 0)
                {
                    this->cmbSourceClass->SelectedIndex =
                        preferredIndex;
                }
                else if (
                    this->cmbSourceClass->Items->Count > 0)
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

        void LoadAcademicYears()
        {
            try
            {
                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT "
                        "academic_year_id, "
                        "year_name "
                        "FROM academic_years "
                        "WHERE status = 'Active' "
                        "ORDER BY academic_year_id DESC"
                    )
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery());

                this->cmbAcademicYear->Items->Clear();

                while (result->next())
                {
                    this->cmbAcademicYear->Items->Add(
                        gcnew ComboItem(
                            result->getInt(
                                "academic_year_id"),
                            gcnew String(
                                result->getString(
                                    "year_name").c_str()),
                            L"",
                            0
                        )
                    );
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
                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT "
                        "term_id, "
                        "term_name "
                        "FROM terms "
                        "WHERE status = 'Active' "
                        "ORDER BY term_id"
                    )
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery());

                this->cmbTerm->Items->Clear();

                while (result->next())
                {
                    this->cmbTerm->Items->Add(
                        gcnew ComboItem(
                            result->getInt("term_id"),
                            gcnew String(
                                result->getString(
                                    "term_name").c_str()),
                            L"",
                            0
                        )
                    );
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
            ComboItem^ source =
                GetSelectedClassItem(
                    this->cmbSourceClass);

            this->cmbTargetClass->Items->Clear();

            if (source == nullptr || source->Grade <= 0)
            {
                UpdateTransitionLayout();
                return;
            }

            int nextGrade = source->Grade + 1;

            try
            {
                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT "
                        "c.class_id, "
                        "c.class_name, "
                        "al.level_code "
                        "FROM classes c "
                        "INNER JOIN academic_levels al "
                        "ON al.academic_level_id = c.academic_level_id "
                        "WHERE c.status = 'Active' "
                        "AND al.status = 'Active' "
                        "ORDER BY c.class_id"
                    )
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery());

                while (result->next())
                {
                    String^ className =
                        gcnew String(
                            result->getString(
                                "class_name").c_str());

                    int grade =
                        GetGradeNumber(className);

                    if (grade != nextGrade)
                    {
                        continue;
                    }

                    this->cmbTargetClass->Items->Add(
                        gcnew ComboItem(
                            result->getInt("class_id"),
                            className,
                            gcnew String(
                                result->getString(
                                    "level_code").c_str()),
                            grade
                        )
                    );
                }

                if (this->cmbTargetClass->Items->Count > 0)
                {
                    this->cmbTargetClass->SelectedIndex = 0;
                }
                else
                {
                    UpdateTransitionLayout();
                    this->studentsGrid->Rows->Clear();

                    this->lblCount->Text =
                        L"No next class is configured for " +
                        source->Text +
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
        }

        void LoadTargetStreams()
        {
            this->streamColumn->Items->Clear();

            ComboItem^ target =
                GetSelectedClassItem(
                    this->cmbTargetClass);

            if (target == nullptr)
            {
                return;
            }

            try
            {
                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT "
                        "stream_id, "
                        "stream_name "
                        "FROM streams "
                        "WHERE class_id = ? "
                        "AND status = 'Active' "
                        "ORDER BY stream_id"
                    )
                );

                stmt->setInt(
                    1,
                    target->Id);

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery());

                while (result->next())
                {
                    this->streamColumn->Items->Add(
                        gcnew ComboItem(
                            result->getInt(
                                "stream_id"),
                            gcnew String(
                                result->getString(
                                    "stream_name").c_str()),
                            L"",
                            0
                        )
                    );
                }

                SetDefaultTargetStreams();
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
            this->option1Column->Items->Clear();
            this->option2Column->Items->Clear();

            if (!IsOLevelTarget())
            {
                return;
            }

            ComboItem^ yearItem =
                GetSelectedClassItem(
                    this->cmbAcademicYear);

            if (this->cmbAcademicYear->SelectedIndex < 0)
            {
                return;
            }

            yearItem =
                dynamic_cast<ComboItem^>(
                    this->cmbAcademicYear->SelectedItem);

            if (yearItem == nullptr)
            {
                return;
            }

            ComboItem^ target =
                GetSelectedClassItem(
                    this->cmbTargetClass);

            if (target == nullptr)
            {
                return;
            }

            try
            {
                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT DISTINCT "
                        "s.subject_id, "
                        "s.subject_name "
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
                        "AND al.level_code = ? "
                        "AND cs.requirement_type = 'Optional' "
                        "AND cs.status = 'Active' "
                        "AND s.status = 'Active' "
                        "ORDER BY s.subject_name"
                    )
                );

                stmt->setInt(
                    1,
                    yearItem->Id);

                stmt->setString(
                    2,
                    "O_LEVEL");

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery());

                while (result->next())
                {
                    ComboItem^ item =
                        gcnew ComboItem(
                            result->getInt(
                                "subject_id"),
                            gcnew String(
                                result->getString(
                                    "subject_name").c_str()),
                            L"",
                            0);

                    this->option1Column->Items->Add(
                        item);

                    this->option2Column->Items->Add(
                        gcnew ComboItem(
                            item->Id,
                            item->Text,
                            L"",
                            0));
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

        void LoadCombinations()
        {
            this->combinationColumn->Items->Clear();

            if (!IsALevelTarget())
            {
                return;
            }

            try
            {
                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT "
                        "combination_id, "
                        "combination_code, "
                        "combination_name "
                        "FROM subject_combinations "
                        "WHERE academic_level_id = ("
                            "SELECT academic_level_id "
                            "FROM academic_levels "
                            "WHERE level_code = 'A_LEVEL' "
                            "AND status = 'Active' "
                            "LIMIT 1"
                        ") "
                        "AND status = 'Active' "
                        "ORDER BY combination_code"
                    )
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery());

                while (result->next())
                {
                    String^ code =
                        gcnew String(
                            result->getString(
                                "combination_code").c_str());

                    String^ name =
                        gcnew String(
                            result->getString(
                                "combination_name").c_str());

                    this->combinationColumn->Items->Add(
                        gcnew ComboItem(
                            result->getInt(
                                "combination_id"),
                            code + L" - " + name,
                            L"A_LEVEL",
                            5));
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

        void UpdateTransitionLayout()
        {
            ComboItem^ source =
                GetSelectedClassItem(
                    this->cmbSourceClass);

            ComboItem^ target =
                GetSelectedClassItem(
                    this->cmbTargetClass);

            this->option1Column->Visible = false;
            this->option2Column->Visible = false;
            this->combinationColumn->Visible = false;

            this->option1Column->ReadOnly = true;
            this->option2Column->ReadOnly = true;
            this->combinationColumn->ReadOnly = true;

            if (source == nullptr ||
                target == nullptr)
            {
                this->lblTitle->Text =
                    L"Bulk Student Promotion";
                this->lblSubtitle->Text =
                    L"Select a current class and load students.";
                this->lblCount->Text = L"";
                return;
            }

            this->lblTitle->Text =
                L"Bulk Student Promotion — " +
                source->Text +
                L" to " +
                target->Text;

            this->lblSubtitle->Text =
                L"Students move to the next configured class. Target streams are taken from " +
                target->Text +
                L"; academic choices are shown only where that class requires them.";

            if (TargetRequiresNewOptions() ||
                TargetCarriesOLevelOptions())
            {
                this->option1Column->Visible = true;
                this->option2Column->Visible = true;
            }

            if (TargetRequiresNewOptions())
            {
                this->option1Column->ReadOnly = false;
                this->option2Column->ReadOnly = false;
            }

            if (TargetRequiresNewCombination() ||
                TargetCarriesCombination())
            {
                this->combinationColumn->Visible = true;
            }

            if (TargetRequiresNewCombination())
            {
                this->combinationColumn->ReadOnly = false;
            }
        }

        void SetDefaultTargetStreams()
        {
            if (this->streamColumn->Items->Count == 0)
            {
                return;
            }

            ComboItem^ first =
                safe_cast<ComboItem^>(
                    this->streamColumn->Items[0]);

            for each (DataGridViewRow^ row in
                this->studentsGrid->Rows)
            {
                row->Cells["TargetStream"]->Value =
                    first;
            }
        }

        void ApplyExistingAcademicChoices()
        {
            bool carryOptions =
                TargetCarriesOLevelOptions();

            bool carryCombination =
                TargetCarriesCombination();

            for each (DataGridViewRow^ row in
                this->studentsGrid->Rows)
            {
                if (carryOptions)
                {
                    int option1Id = 0;
                    int option2Id = 0;

                    Object^ option1Value =
                        row->Cells["Option1"]->Tag;

                    Object^ option2Value =
                        row->Cells["Option2"]->Tag;

                    if (option1Value != nullptr)
                    {
                        option1Id =
                            Convert::ToInt32(option1Value);
                    }

                    if (option2Value != nullptr)
                    {
                        option2Id =
                            Convert::ToInt32(option2Value);
                    }

                    ComboItem^ option1 =
                        FindItemById(
                            this->option1Column,
                            option1Id);

                    ComboItem^ option2 =
                        FindItemById(
                            this->option2Column,
                            option2Id);

                    row->Cells["Option1"]->Value =
                        option1;

                    row->Cells["Option2"]->Value =
                        option2;
                }

                if (carryCombination)
                {
                    Object^ comboValue =
                        row->Cells["Combination"]->Tag;

                    if (comboValue != nullptr)
                    {
                        int combinationId =
                            Convert::ToInt32(comboValue);

                        row->Cells["Combination"]->Value =
                            FindItemById(
                                this->combinationColumn,
                                combinationId);
                    }
                }
            }
        }

        void LoadStudents()
        {
            this->studentsGrid->Rows->Clear();

            ComboItem^ source =
                GetSelectedClassItem(
                    this->cmbSourceClass);

            if (source == nullptr)
            {
                this->lblCount->Text =
                    L"Select a current class.";
                return;
            }

            try
            {
                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT "
                        "e.enrollment_id, "
                        "s.student_id, "
                        "s.registration_number, "
                        "CONCAT_WS(' ', "
                            "s.first_name, "
                            "s.middle_name, "
                            "s.last_name"
                        ") AS full_name, "
                        "c.class_name, "
                        "st.stream_name, "
                        "e.combination_id, "
                        "MAX(CASE "
                            "WHEN eos.option_number = 1 "
                            "THEN eos.subject_id "
                            "END) AS option1_id, "
                        "MAX(CASE "
                            "WHEN eos.option_number = 2 "
                            "THEN eos.subject_id "
                            "END) AS option2_id "
                        "FROM enrollments e "
                        "INNER JOIN students s "
                        "ON s.student_id = e.student_id "
                        "INNER JOIN classes c "
                        "ON c.class_id = e.class_id "
                        "LEFT JOIN streams st "
                        "ON st.stream_id = e.stream_id "
                        "LEFT JOIN enrollment_optional_subjects eos "
                        "ON eos.enrollment_id = e.enrollment_id "
                        "AND eos.status = 'Active' "
                        "WHERE e.class_id = ? "
                        "AND e.status = 'Active' "
                        "GROUP BY "
                            "e.enrollment_id, "
                            "s.student_id, "
                            "s.registration_number, "
                            "s.first_name, "
                            "s.middle_name, "
                            "s.last_name, "
                            "c.class_name, "
                            "st.stream_name, "
                            "e.combination_id "
                        "ORDER BY "
                            "s.last_name, "
                            "s.first_name, "
                            "s.student_id"
                    )
                );

                stmt->setInt(
                    1,
                    source->Id);

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery());

                while (result->next())
                {
                    bool preselected = false;

                    if (this->studentIds != nullptr)
                    {
                        long long id =
                            result->getInt64(
                                "student_id");

                        for each (
                            long long selectedId
                            in this->studentIds)
                        {
                            if (selectedId == id)
                            {
                                preselected = true;
                                break;
                            }
                        }
                    }

                    int rowIndex =
                        this->studentsGrid->Rows->Add(
                            preselected,
                            result->getInt(
                                "enrollment_id"),
                            result->getInt64(
                                "student_id"),
                            gcnew String(
                                result->getString(
                                    "registration_number").c_str()),
                            gcnew String(
                                result->getString(
                                    "full_name").c_str()),
                            gcnew String(
                                result->getString(
                                    "class_name").c_str()),
                            result->isNull("stream_name")
                                ? L"Not assigned"
                                : gcnew String(
                                    result->getString(
                                        "stream_name").c_str()),
                            nullptr,
                            nullptr,
                            nullptr,
                            nullptr);

                    if (!result->isNull("option1_id"))
                    {
                        this->studentsGrid
                            ->Rows[rowIndex]
                            ->Cells["Option1"]
                            ->Tag =
                                result->getInt(
                                    "option1_id");
                    }

                    if (!result->isNull("option2_id"))
                    {
                        this->studentsGrid
                            ->Rows[rowIndex]
                            ->Cells["Option2"]
                            ->Tag =
                                result->getInt(
                                    "option2_id");
                    }

                    if (!result->isNull("combination_id"))
                    {
                        this->studentsGrid
                            ->Rows[rowIndex]
                            ->Cells["Combination"]
                            ->Tag =
                                result->getInt(
                                    "combination_id");
                    }
                }

                this->lblCount->Text =
                    L"Students in " +
                    source->Text +
                    L": " +
                    this->studentsGrid->Rows->Count.ToString() +
                    L"    |    Select the students to promote.";
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error);
            }

            SetDefaultTargetStreams();
            ApplyExistingAcademicChoices();
        }

        void ClearStudentSelections()
        {
            for each (DataGridViewRow^ row in
                this->studentsGrid->Rows)
            {
                row->Cells["Select"]->Value = false;
            }
        }

        System::Void cmbSourceClass_SelectedIndexChanged(
            Object^ sender,
            EventArgs^ e)
        {
            this->studentIds = nullptr;
            LoadTargetClasses();
            UpdateTransitionLayout();
            LoadTargetStreams();
            LoadOptionalSubjects();
            LoadCombinations();
            LoadStudents();
        }

        System::Void cmbTargetClass_SelectedIndexChanged(
            Object^ sender,
            EventArgs^ e)
        {
            UpdateTransitionLayout();
            LoadTargetStreams();
            LoadOptionalSubjects();
            LoadCombinations();
            ApplyExistingAcademicChoices();
        }

        System::Void cmbAcademicYear_SelectedIndexChanged(
            Object^ sender,
            EventArgs^ e)
        {
            LoadOptionalSubjects();
            ApplyExistingAcademicChoices();
        }

        System::Void btnLoadClass_Click(
            Object^ sender,
            EventArgs^ e)
        {
            LoadStudents();
        }

        System::Void studentsGrid_CurrentCellDirtyStateChanged(
            Object^ sender,
            EventArgs^ e)
        {
            if (!this->studentsGrid->IsCurrentCellDirty ||
                this->studentsGrid->CurrentCell == nullptr)
            {
                return;
            }

            if (this->studentsGrid->CurrentCell->OwningColumn->Name
                ->Equals(
                    L"Select",
                    StringComparison::Ordinal))
            {
                this->studentsGrid->CommitEdit(
                    DataGridViewDataErrorContexts::Commit);
            }
        }

        System::Void studentsGrid_CellValueChanged(
            Object^ sender,
            DataGridViewCellEventArgs^ e)
        {
            if (e->RowIndex < 0)
            {
                return;
            }

            if (e->ColumnIndex ==
                this->studentsGrid->Columns["Select"]->Index)
            {
                int selected = 0;

                for each (DataGridViewRow^ row in
                    this->studentsGrid->Rows)
                {
                    if (row->Cells["Select"]->Value != nullptr &&
                        Convert::ToBoolean(
                            row->Cells["Select"]->Value))
                    {
                        selected++;
                    }
                }

                this->lblCount->Text =
                    L"Students selected: " +
                    selected.ToString() +
                    L" / " +
                    this->studentsGrid->Rows->Count.ToString();
            }
        }

        System::Void btnPromote_Click(
            Object^ sender,
            EventArgs^ e)
        {
            int selectedCount = 0;

            for each (DataGridViewRow^ row in
                this->studentsGrid->Rows)
            {
                if (row->Cells["Select"]->Value != nullptr &&
                    Convert::ToBoolean(
                        row->Cells["Select"]->Value))
                {
                    selectedCount++;
                }
            }

            if (selectedCount == 0)
            {
                MessageBox::Show(
                    L"Select at least one student to promote.",
                    L"Bulk Promotion",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning);
                return;
            }

            ComboItem^ source =
                GetSelectedClassItem(
                    this->cmbSourceClass);

            ComboItem^ target =
                GetSelectedClassItem(
                    this->cmbTargetClass);

            ComboItem^ year =
                this->cmbAcademicYear->SelectedIndex >= 0
                    ? dynamic_cast<ComboItem^>(
                        this->cmbAcademicYear->SelectedItem)
                    : nullptr;

            ComboItem^ term =
                this->cmbTerm->SelectedIndex >= 0
                    ? dynamic_cast<ComboItem^>(
                        this->cmbTerm->SelectedItem)
                    : nullptr;

            if (source == nullptr ||
                target == nullptr ||
                year == nullptr ||
                term == nullptr)
            {
                MessageBox::Show(
                    L"Select the current class, target class, academic year and term.",
                    L"Bulk Promotion",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning);
                return;
            }

            if (target->Grade != source->Grade + 1)
            {
                MessageBox::Show(
                    L"Students can only be promoted to the next class.",
                    L"Bulk Promotion",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning);
                return;
            }

            for each (DataGridViewRow^ row in
                this->studentsGrid->Rows)
            {
                if (row->Cells["Select"]->Value == nullptr ||
                    !Convert::ToBoolean(
                        row->Cells["Select"]->Value))
                {
                    continue;
                }

                if (this->streamColumn->Items->Count > 0 &&
                    row->Cells["TargetStream"]->Value == nullptr)
                {
                    MessageBox::Show(
                        L"Every selected student must have a target stream.",
                        L"Bulk Promotion",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Warning);
                    return;
                }

                if (TargetRequiresNewOptions())
                {
                    ComboItem^ option1 =
                        dynamic_cast<ComboItem^>(
                            row->Cells["Option1"]->Value);

                    ComboItem^ option2 =
                        dynamic_cast<ComboItem^>(
                            row->Cells["Option2"]->Value);

                    if (option1 == nullptr ||
                        option2 == nullptr)
                    {
                        MessageBox::Show(
                            L"Every selected Senior 2 student must have two optional subjects.",
                            L"Bulk Promotion",
                            MessageBoxButtons::OK,
                            MessageBoxIcon::Warning);
                        return;
                    }

                    if (option1->Id == option2->Id)
                    {
                        MessageBox::Show(
                            L"The two optional subjects must be different.",
                            L"Bulk Promotion",
                            MessageBoxButtons::OK,
                            MessageBoxIcon::Warning);
                        return;
                    }
                }

                if (TargetCarriesOLevelOptions())
                {
                    if (row->Cells["Option1"]->Value == nullptr ||
                        row->Cells["Option2"]->Value == nullptr)
                    {
                        MessageBox::Show(
                            L"One or more selected Senior 3 students has no recorded optional-subject choices to carry into Senior 4.",
                            L"Bulk Promotion",
                            MessageBoxButtons::OK,
                            MessageBoxIcon::Warning);
                        return;
                    }
                }

                if (TargetRequiresNewCombination() ||
                    TargetCarriesCombination())
                {
                    if (row->Cells["Combination"]->Value == nullptr)
                    {
                        MessageBox::Show(
                            L"Every selected A-Level student must have an A-Level combination.",
                            L"Bulk Promotion",
                            MessageBoxButtons::OK,
                            MessageBoxIcon::Warning);
                        return;
                    }
                }
            }

            String^ confirmation =
                L"You are about to promote " +
                selectedCount.ToString() +
                L" student(s) from " +
                source->Text +
                L" to " +
                target->Text +
                L".\n\n"
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

            auto con =
                DbConnection::GetConnection();

            try
            {
                con->setAutoCommit(false);

                for each (DataGridViewRow^ row in
                    this->studentsGrid->Rows)
                {
                    if (row->Cells["Select"]->Value == nullptr ||
                        !Convert::ToBoolean(
                            row->Cells["Select"]->Value))
                    {
                        continue;
                    }

                    long long studentId =
                        Convert::ToInt64(
                            row->Cells["StudentId"]->Value);

                    int currentEnrollmentId =
                        Convert::ToInt32(
                            row->Cells["EnrollmentId"]->Value);

                    ComboItem^ targetStream =
                        dynamic_cast<ComboItem^>(
                            row->Cells["TargetStream"]->Value);

                    ComboItem^ option1 =
                        dynamic_cast<ComboItem^>(
                            row->Cells["Option1"]->Value);

                    ComboItem^ option2 =
                        dynamic_cast<ComboItem^>(
                            row->Cells["Option2"]->Value);

                    ComboItem^ combination =
                        dynamic_cast<ComboItem^>(
                            row->Cells["Combination"]->Value);

                    std::unique_ptr<sql::PreparedStatement> duplicateCheck(
                        con->prepareStatement(
                            "SELECT enrollment_id "
                            "FROM enrollments "
                            "WHERE student_id = ? "
                            "AND academic_year_id = ? "
                            "AND term_id = ? "
                            "LIMIT 1"
                        )
                    );

                    duplicateCheck->setInt64(
                        1,
                        studentId);

                    duplicateCheck->setInt(
                        2,
                        year->Id);

                    duplicateCheck->setInt(
                        3,
                        term->Id);

                    std::unique_ptr<sql::ResultSet> duplicateResult(
                        duplicateCheck->executeQuery());

                    if (duplicateResult->next())
                    {
                        throw std::runtime_error(
                            "A selected student already has an enrollment for the target academic period.");
                    }

                    int combinationId =
                        combination != nullptr
                            ? combination->Id
                            : 0;

                    std::unique_ptr<sql::PreparedStatement> insertEnrollment(
                        con->prepareStatement(
                            "INSERT INTO enrollments "
                            "(student_id, academic_year_id, term_id, class_id, stream_id, combination_id, enrollment_date, status) "
                            "VALUES (?, ?, ?, ?, ?, ?, ?, 'Active')"
                        )
                    );

                    insertEnrollment->setInt64(
                        1,
                        studentId);

                    insertEnrollment->setInt(
                        2,
                        year->Id);

                    insertEnrollment->setInt(
                        3,
                        term->Id);

                    insertEnrollment->setInt(
                        4,
                        target->Id);

                    if (targetStream != nullptr)
                    {
                        insertEnrollment->setInt(
                            5,
                            targetStream->Id);
                    }
                    else
                    {
                        insertEnrollment->setNull(
                            5,
                            sql::DataType::INTEGER);
                    }

                    if (combinationId > 0)
                    {
                        insertEnrollment->setInt(
                            6,
                            combinationId);
                    }
                    else
                    {
                        insertEnrollment->setNull(
                            6,
                            sql::DataType::INTEGER);
                    }

                    insertEnrollment->setString(
                        7,
                        msclr::interop::marshal_as<std::string>(
                            DateTime::Today.ToString(
                                L"yyyy-MM-dd")));

                    insertEnrollment->executeUpdate();

                    int newEnrollmentId = 0;

                    std::unique_ptr<sql::Statement> keyStmt(
                        con->createStatement());

                    std::unique_ptr<sql::ResultSet> keys(
                        keyStmt->executeQuery(
                            "SELECT LAST_INSERT_ID() AS enrollment_id"));

                    if (!keys->next())
                    {
                        throw std::runtime_error(
                            "Could not obtain the new enrollment ID.");
                    }

                    newEnrollmentId =
                        keys->getInt("enrollment_id");

                    if (TargetRequiresNewOptions() ||
                        TargetCarriesOLevelOptions())
                    {
                        std::unique_ptr<sql::PreparedStatement> insertOption(
                            con->prepareStatement(
                                "INSERT INTO enrollment_optional_subjects "
                                "(enrollment_id, subject_id, option_number, selected_date, status) "
                                "VALUES (?, ?, ?, ?, 'Active')"
                            )
                        );

                        insertOption->setInt(
                            1,
                            newEnrollmentId);

                        insertOption->setInt(
                            2,
                            option1->Id);

                        insertOption->setInt(
                            3,
                            1);

                        insertOption->setString(
                            4,
                            msclr::interop::marshal_as<std::string>(
                                DateTime::Today.ToString(
                                    L"yyyy-MM-dd")));

                        insertOption->executeUpdate();

                        insertOption->setInt(
                            1,
                            newEnrollmentId);

                        insertOption->setInt(
                            2,
                            option2->Id);

                        insertOption->setInt(
                            3,
                            2);

                        insertOption->setString(
                            4,
                            msclr::interop::marshal_as<std::string>(
                                DateTime::Today.ToString(
                                    L"yyyy-MM-dd")));

                        insertOption->executeUpdate();
                    }

                    std::unique_ptr<sql::PreparedStatement> closeOld(
                        con->prepareStatement(
                            "UPDATE enrollments "
                            "SET status = 'Completed' "
                            "WHERE enrollment_id = ? "
                            "AND student_id = ? "
                            "AND status = 'Active'"
                        )
                    );

                    closeOld->setInt(
                        1,
                        currentEnrollmentId);

                    closeOld->setInt64(
                        2,
                        studentId);

                    closeOld->executeUpdate();
                }

                con->commit();
                con->setAutoCommit(true);

                MessageBox::Show(
                    L"All selected students were promoted successfully.",
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

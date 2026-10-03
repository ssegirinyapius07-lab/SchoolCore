#pragma once

#include "DbConnection.h"

#include <mariadb/conncpp.hpp>
#include <msclr/marshal_cppstd.h>

#include <memory>
#include <string>

using namespace System;
using namespace System::ComponentModel;
using namespace System::Drawing;
using namespace System::Windows::Forms;

namespace SchoolCore
{
    public ref class ClassesStreams : public System::Windows::Forms::Form
    {
    public:
                ClassesStreams(void)
                {
                    InitializeComponent();
        
                    this->btnAddClass->Click +=
                        gcnew System::EventHandler(
                            this,
                            &ClassesStreams::btnAddClass_Click
                        );
        
                    this->btnToggleClass->Click +=
                        gcnew System::EventHandler(
                            this,
                            &ClassesStreams::btnToggleClass_Click
                        );
        
                    this->classesGrid->SelectionChanged +=
                        gcnew System::EventHandler(
                            this,
                            &ClassesStreams::classesGrid_SelectionChanged
                        );
        
                    this->cmbAcademicYear->SelectedIndexChanged +=
                        gcnew System::EventHandler(
                            this,
                            &ClassesStreams::cmbAcademicYear_SelectedIndexChanged
                        );
        
                    this->cmbTerm->SelectedIndexChanged +=
                        gcnew System::EventHandler(
                            this,
                            &ClassesStreams::cmbTerm_SelectedIndexChanged
                        );
        
                    this->btnAddStream->Click +=
                        gcnew System::EventHandler(
                            this,
                            &ClassesStreams::btnAddStream_Click
                        );
        
                    this->btnToggleStream->Click +=
                        gcnew System::EventHandler(
                            this,
                            &ClassesStreams::btnToggleStream_Click
                        );
        
                    this->streamsGrid->SelectionChanged +=
                        gcnew System::EventHandler(
                            this,
                            &ClassesStreams::streamsGrid_SelectionChanged
                        );
        
               
        
        
                    if (System::ComponentModel::LicenseManager::UsageMode != System::ComponentModel::LicenseUsageMode::Designtime)
                    {
                        LoadAcademicYears();
                        LoadClasses();
                    }
                }

        System::ComponentModel::Container^ components;


    protected:
        ~ClassesStreams(void)
        {
            if (this->components)
            {
                delete this->components;
            }
        }

    private:

        // =========================================================
        // CONTROLS
        // =========================================================

        System::Windows::Forms::TableLayoutPanel^ mainLayout;

        System::Windows::Forms::Panel^ headerPanel;
        System::Windows::Forms::Label^ lblTitle;
        System::Windows::Forms::Label^ lblSubtitle;

        System::Windows::Forms::GroupBox^ classGroup;
        System::Windows::Forms::GroupBox^ streamGroup;

        // Classes
        System::Windows::Forms::Label^ lblClassName;
        System::Windows::Forms::ComboBox^ cmbClassName;
        System::Windows::Forms::Button^ btnAddClass;
        System::Windows::Forms::Button^ btnToggleClass;
        System::Windows::Forms::DataGridView^ classesGrid;

        // Streams
        System::Windows::Forms::Label^ lblSelectedClass;
        System::Windows::Forms::Label^ lblSelectedClassValue;
        System::Windows::Forms::Label^ lblAcademicYear;
        System::Windows::Forms::Label^ lblTerm;
        System::Windows::Forms::Label^ lblStreamName;

        System::Windows::Forms::ComboBox^ cmbAcademicYear;
        System::Windows::Forms::ComboBox^ cmbTerm;

        System::Windows::Forms::TextBox^ txtStreamName;

        System::Windows::Forms::Button^ btnAddStream;
        System::Windows::Forms::Button^ btnToggleStream;

        System::Windows::Forms::DataGridView^ streamsGrid;

        // Bottom
        System::Windows::Forms::Panel^ buttonPanel;


        // =========================================================
        // COMBO ITEM
        // =========================================================

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


        void StyleProfessionalGrid(
            System::Windows::Forms::DataGridView^ grid)
        {
            grid->BackgroundColor =
                Color::White;

            grid->BorderStyle =
                BorderStyle::None;

            grid->EnableHeadersVisualStyles =
                false;

            grid->ColumnHeadersDefaultCellStyle->BackColor =
                Color::FromArgb(30, 41, 59);

            grid->ColumnHeadersDefaultCellStyle->ForeColor =
                Color::White;

            grid->ColumnHeadersDefaultCellStyle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    9.0F,
                    FontStyle::Bold
                );

            grid->ColumnHeadersHeight =
                36;

            grid->DefaultCellStyle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.5F,
                    FontStyle::Regular
                );

            grid->DefaultCellStyle->SelectionBackColor =
                Color::FromArgb(219, 234, 254);

            grid->DefaultCellStyle->SelectionForeColor =
                Color::FromArgb(30, 41, 59);

            grid->AlternatingRowsDefaultCellStyle->BackColor =
                Color::FromArgb(248, 250, 252);

            grid->RowTemplate->Height =
                32;

            grid->RowHeadersVisible =
                false;
        }

        // =========================================================
        // INITIALIZE COMPONENTS
        // =========================================================

        #pragma region Windows Form Designer generated code

void InitializeComponent(void)
        {
            this->components = gcnew System::ComponentModel::Container();
            this->SuspendLayout();

            // =====================================================
            // FORM
            // =====================================================

            this->Text = L"Classes & Streams";

            this->StartPosition =
                System::Windows::Forms::FormStartPosition::CenterParent;
            this->WindowState =
                System::Windows::Forms::FormWindowState::Normal;

            this->ClientSize =
                System::Drawing::Size(1000, 620);

            this->MinimumSize =
                System::Drawing::Size(900, 600);

            this->BackColor =
                System::Drawing::Color::WhiteSmoke;

            this->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.5F,
                    System::Drawing::FontStyle::Regular
                );


            // =====================================================
            // MAIN LAYOUT
            // =====================================================

            this->mainLayout =
                gcnew System::Windows::Forms::TableLayoutPanel();

            this->mainLayout->Dock =
                System::Windows::Forms::DockStyle::Fill;

            this->mainLayout->AutoScroll = true;

            this->mainLayout->ColumnCount = 1;
            this->mainLayout->RowCount = 3;

            this->mainLayout->Padding =
                System::Windows::Forms::Padding(18);

            this->mainLayout->BackColor =
                System::Drawing::Color::WhiteSmoke;


            this->mainLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    System::Windows::Forms::SizeType::Percent,
                    100.0F
                )
            );

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    System::Windows::Forms::SizeType::Absolute,
                    78.0F
                )
            );

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    System::Windows::Forms::SizeType::Percent,
                    50.0F
                )
            );

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    System::Windows::Forms::SizeType::Percent,
                    50.0F
                )
            );

          


            // =====================================================
            // HEADER
            // =====================================================

            this->headerPanel =
                gcnew System::Windows::Forms::Panel();

            this->headerPanel->Dock =
                System::Windows::Forms::DockStyle::Fill;

            this->headerPanel->BackColor =
                System::Drawing::Color::FromArgb(
                    35, 47, 62
                );

            this->headerPanel->Padding =
                System::Windows::Forms::Padding(
                    20, 10, 20, 10
                );


            this->lblTitle =
                gcnew System::Windows::Forms::Label();

            this->lblTitle->AutoSize = true;

            this->lblTitle->Text =
                L"Classes & Streams";

            this->lblTitle->ForeColor =
                System::Drawing::Color::White;

            this->lblTitle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    18.0F,
                    System::Drawing::FontStyle::Bold
                );

            this->lblTitle->Location =
                System::Drawing::Point(18, 10);


            this->lblSubtitle =
                gcnew System::Windows::Forms::Label();

            this->lblSubtitle->AutoSize = true;

            this->lblSubtitle->Text =
                L"Manage the Senior 1–Senior 6 classes and monitor live student enrollment.";

            this->lblSubtitle->ForeColor =
                System::Drawing::Color::FromArgb(
                    220, 225, 230
                );

            this->lblSubtitle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.5F,
                    System::Drawing::FontStyle::Regular
                );

            this->lblSubtitle->Location =
                System::Drawing::Point(20, 45);


            this->headerPanel->Controls->Add(
                this->lblTitle
            );

            this->headerPanel->Controls->Add(
                this->lblSubtitle
            );


            // =====================================================
            // CLASS GROUP
            // =====================================================

            this->classGroup =
                gcnew System::Windows::Forms::GroupBox();

            this->classGroup->Dock =
                System::Windows::Forms::DockStyle::Fill;

            this->classGroup->Text =
                L"Classes";

            this->classGroup->BackColor =
                System::Drawing::Color::White;

            this->classGroup->ForeColor =
                System::Drawing::Color::FromArgb(30, 41, 59);

            this->classGroup->Margin =
                System::Windows::Forms::Padding(
                    0, 0, 0, 10
                );

            this->classGroup->Padding =
                System::Windows::Forms::Padding(12);

            this->classGroup->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    10.0F,
                    System::Drawing::FontStyle::Bold
                );


            System::Windows::Forms::TableLayoutPanel^ classLayout =
                gcnew System::Windows::Forms::TableLayoutPanel();

            classLayout->Dock =
                System::Windows::Forms::DockStyle::Fill;

            classLayout->ColumnCount = 4;
            classLayout->RowCount = 2;

            classLayout->Padding =
                System::Windows::Forms::Padding(5);


            classLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    System::Windows::Forms::SizeType::Absolute,
                    110.0F
                )
            );

            classLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    System::Windows::Forms::SizeType::Percent,
                    55.0F
                )
            );

            classLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    System::Windows::Forms::SizeType::Absolute,
                    150.0F
                )
            );

            classLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    System::Windows::Forms::SizeType::Percent,
                    45.0F
                )
            );


            classLayout->RowStyles->Add(
                gcnew RowStyle(
                    System::Windows::Forms::SizeType::Absolute,
                    45.0F
                )
            );

            classLayout->RowStyles->Add(
                gcnew RowStyle(
                    System::Windows::Forms::SizeType::Percent,
                    100.0F
                )
            );


            // Class label

            this->lblClassName =
                gcnew System::Windows::Forms::Label();

            this->lblClassName->Text =
                L"Class Name";

            this->lblClassName->Dock =
                System::Windows::Forms::DockStyle::Fill;

            this->lblClassName->TextAlign =
                System::Drawing::ContentAlignment::MiddleLeft;


            // Class input

            this->cmbClassName =
                gcnew System::Windows::Forms::ComboBox();

            this->cmbClassName->Dock =
                System::Windows::Forms::DockStyle::Fill;

            this->cmbClassName->DropDownStyle =
                System::Windows::Forms::ComboBoxStyle::DropDownList;

            this->cmbClassName->Items->Add(
                L"Senior 1"
            );

            this->cmbClassName->Items->Add(
                L"Senior 2"
            );

            this->cmbClassName->Items->Add(
                L"Senior 3"
            );

            this->cmbClassName->Items->Add(
                L"Senior 4"
            );

            this->cmbClassName->Items->Add(
                L"Senior 5"
            );

            this->cmbClassName->Items->Add(
                L"Senior 6"
            );

            this->cmbClassName->SelectedIndex = 0;

            this->cmbClassName->Margin =
                System::Windows::Forms::Padding(3);


            // Add class

            this->btnAddClass =
                gcnew System::Windows::Forms::Button();

            this->btnAddClass->Text =
                L"+ Add Class";

            this->btnAddClass->Dock =
                System::Windows::Forms::DockStyle::Fill;


            // Toggle class

            this->btnToggleClass =
                gcnew System::Windows::Forms::Button();

            this->btnToggleClass->Text =
                L"Activate / Deactivate";

            this->btnToggleClass->Dock =
                System::Windows::Forms::DockStyle::Fill;


            // Classes grid

            this->classesGrid =
                gcnew System::Windows::Forms::DataGridView();

            this->classesGrid->Dock =
                System::Windows::Forms::DockStyle::Fill;

            this->classesGrid->AllowUserToAddRows =
                false;

            this->classesGrid->AllowUserToDeleteRows =
                false;

            this->classesGrid->ReadOnly =
                true;

            this->classesGrid->SelectionMode =
                System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;

            this->classesGrid->MultiSelect =
                false;

            this->classesGrid->AutoSizeColumnsMode =
                System::Windows::Forms::DataGridViewAutoSizeColumnsMode::Fill;

            this->classesGrid->RowHeadersVisible =
                false;


            this->classesGrid->Columns->Add(
                L"id",
                L"ID"
            );

            this->classesGrid->Columns->Add(
                L"class_name",
                L"Class"
            );

            this->classesGrid->Columns->Add(
                L"level_name",
                L"Academic Level"
            );

            this->classesGrid->Columns->Add(
                L"student_count",
                L"Students"
            );

            this->classesGrid->Columns->Add(
                L"status",
                L"Status"
            );

            StyleProfessionalGrid(
                this->classesGrid
            );

            this->classesGrid->Columns[0]->Visible =
                false;


            // Add controls

            classLayout->Controls->Add(
                this->lblClassName,
                0, 0
            );

            classLayout->Controls->Add(
                this->cmbClassName,
                1, 0
            );

            classLayout->Controls->Add(
                this->btnAddClass,
                2, 0
            );

            classLayout->Controls->Add(
                this->btnToggleClass,
                3, 0
            );


            classLayout->Controls->Add(
                this->classesGrid,
                0, 1
            );

            classLayout->SetColumnSpan(
                this->classesGrid,
                4
            );


            this->classGroup->Controls->Add(
                classLayout
            );


            // =====================================================
            // STREAM GROUP
            // =====================================================

            this->streamGroup =
                gcnew System::Windows::Forms::GroupBox();

            this->streamGroup->Dock =
                System::Windows::Forms::DockStyle::Fill;

            this->streamGroup->Text =
                L"Streams";

            this->streamGroup->BackColor =
                System::Drawing::Color::White;

            this->streamGroup->ForeColor =
                System::Drawing::Color::FromArgb(30, 41, 59);

            this->streamGroup->Margin =
                System::Windows::Forms::Padding(
                    0
                );

            this->streamGroup->Padding =
                System::Windows::Forms::Padding(12);

            this->streamGroup->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    10.0F,
                    System::Drawing::FontStyle::Bold
                );


            System::Windows::Forms::TableLayoutPanel^ streamLayout =
                gcnew System::Windows::Forms::TableLayoutPanel();

            streamLayout->Dock =
                System::Windows::Forms::DockStyle::Fill;

            streamLayout->ColumnCount = 6;
            streamLayout->RowCount = 3;

            streamLayout->Padding =
                System::Windows::Forms::Padding(5);


            streamLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    System::Windows::Forms::SizeType::Absolute,
                    105.0F
                )
            );

            streamLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    System::Windows::Forms::SizeType::Percent,
                    30.0F
                )
            );

            streamLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    System::Windows::Forms::SizeType::Absolute,
                    105.0F
                )
            );

            streamLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    System::Windows::Forms::SizeType::Percent,
                    25.0F
                )
            );

            streamLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    System::Windows::Forms::SizeType::Absolute,
                    90.0F
                )
            );

            streamLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    System::Windows::Forms::SizeType::Percent,
                    45.0F
                )
            );


            streamLayout->RowStyles->Add(
                gcnew RowStyle(
                    System::Windows::Forms::SizeType::Absolute,
                    40.0F
                )
            );

            streamLayout->RowStyles->Add(
                gcnew RowStyle(
                    System::Windows::Forms::SizeType::Absolute,
                    40.0F
                )
            );

            streamLayout->RowStyles->Add(
                gcnew RowStyle(
                    System::Windows::Forms::SizeType::Percent,
                    100.0F
                )
            );


            // Selected class

            this->lblSelectedClass =
                gcnew System::Windows::Forms::Label();

            this->lblSelectedClass->Text =
                L"Class";

            this->lblSelectedClass->Dock =
                System::Windows::Forms::DockStyle::Fill;

            this->lblSelectedClass->TextAlign =
                System::Drawing::ContentAlignment::MiddleLeft;


            this->lblSelectedClassValue =
                gcnew System::Windows::Forms::Label();

            this->lblSelectedClassValue->Text =
                L"No class selected.";

            this->lblSelectedClassValue->Dock =
                System::Windows::Forms::DockStyle::Fill;

            this->lblSelectedClassValue->TextAlign =
                System::Drawing::ContentAlignment::MiddleLeft;

            this->lblSelectedClassValue->ForeColor =
                System::Drawing::Color::DimGray;


            // Academic year

            this->lblAcademicYear =
                gcnew System::Windows::Forms::Label();

            this->lblAcademicYear->Text =
                L"Academic Year";

            this->lblAcademicYear->Dock =
                System::Windows::Forms::DockStyle::Fill;

            this->lblAcademicYear->TextAlign =
                System::Drawing::ContentAlignment::MiddleLeft;


            this->cmbAcademicYear =
                gcnew System::Windows::Forms::ComboBox();

            this->cmbAcademicYear->Dock =
                System::Windows::Forms::DockStyle::Fill;

            this->cmbAcademicYear->DropDownStyle =
                System::Windows::Forms::ComboBoxStyle::DropDownList;


            // Term

            this->lblTerm =
                gcnew System::Windows::Forms::Label();

            this->lblTerm->Text =
                L"Term";

            this->lblTerm->Dock =
                System::Windows::Forms::DockStyle::Fill;

            this->lblTerm->TextAlign =
                System::Drawing::ContentAlignment::MiddleLeft;


            this->cmbTerm =
                gcnew System::Windows::Forms::ComboBox();

            this->cmbTerm->Dock =
                System::Windows::Forms::DockStyle::Fill;

            this->cmbTerm->DropDownStyle =
                System::Windows::Forms::ComboBoxStyle::DropDownList;

            this->cmbTerm->Enabled =
                false;


            // Stream name

            this->lblStreamName =
                gcnew System::Windows::Forms::Label();

            this->lblStreamName->Text =
                L"Stream";

            this->lblStreamName->Dock =
                System::Windows::Forms::DockStyle::Fill;

            this->lblStreamName->TextAlign =
                System::Drawing::ContentAlignment::MiddleLeft;


            this->txtStreamName =
                gcnew System::Windows::Forms::TextBox();

            this->txtStreamName->Dock =
                System::Windows::Forms::DockStyle::Fill;

            this->txtStreamName->Margin =
                System::Windows::Forms::Padding(3);


            // Add stream

            this->btnAddStream =
                gcnew System::Windows::Forms::Button();

            this->btnAddStream->Text =
                L"+ Add Stream";

            this->btnAddStream->Dock =
                System::Windows::Forms::DockStyle::Fill;

            this->btnAddStream->Enabled =
                false;


            // Toggle stream

            this->btnToggleStream =
                gcnew System::Windows::Forms::Button();

            this->btnToggleStream->Text =
                L"Activate / Deactivate";

            this->btnToggleStream->Dock =
                System::Windows::Forms::DockStyle::Fill;

            this->btnToggleStream->Enabled =
                false;


            // Stream grid

            this->streamsGrid =
                gcnew System::Windows::Forms::DataGridView();

            this->streamsGrid->Dock =
                System::Windows::Forms::DockStyle::Fill;

            this->streamsGrid->AllowUserToAddRows =
                false;

            this->streamsGrid->AllowUserToDeleteRows =
                false;

            this->streamsGrid->ReadOnly =
                true;

            this->streamsGrid->SelectionMode =
                System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;

            this->streamsGrid->MultiSelect =
                false;

            this->streamsGrid->AutoSizeColumnsMode =
                System::Windows::Forms::DataGridViewAutoSizeColumnsMode::Fill;

            this->streamsGrid->RowHeadersVisible =
                false;


            this->streamsGrid->Columns->Add(
                L"id",
                L"ID"
            );

            this->streamsGrid->Columns->Add(
                L"stream_name",
                L"Stream"
            );

            this->streamsGrid->Columns->Add(
                L"student_count",
                L"Students"
            );

            this->streamsGrid->Columns->Add(
                L"status",
                L"Status"
            );

            StyleProfessionalGrid(
                this->streamsGrid
            );

            this->streamsGrid->Columns[0]->Visible =
                false;


            // Row 0

            streamLayout->Controls->Add(
                this->lblSelectedClass,
                0, 0
            );

            streamLayout->Controls->Add(
                this->lblSelectedClassValue,
                1, 0
            );

            streamLayout->Controls->Add(
                this->lblAcademicYear,
                2, 0
            );

            streamLayout->Controls->Add(
                this->cmbAcademicYear,
                3, 0
            );

            streamLayout->Controls->Add(
                this->lblTerm,
                4, 0
            );

            streamLayout->Controls->Add(
                this->cmbTerm,
                5, 0
            );


            // Row 1

            streamLayout->Controls->Add(
                this->lblStreamName,
                0, 1
            );

            streamLayout->Controls->Add(
                this->txtStreamName,
                1, 1
            );

            streamLayout->SetColumnSpan(
                this->txtStreamName,
                2
            );


            streamLayout->Controls->Add(
                this->btnAddStream,
                3, 1
            );

            streamLayout->Controls->Add(
                this->btnToggleStream,
                4, 1
            );


            // Row 2

            streamLayout->Controls->Add(
                this->streamsGrid,
                0, 2
            );

            streamLayout->SetColumnSpan(
                this->streamsGrid,
                6
            );


            this->streamGroup->Controls->Add(
                streamLayout
            );


            // =====================================================
            // BUTTON PANEL
            // =====================================================

            


            // =====================================================
            // MAIN LAYOUT
            // =====================================================

            this->mainLayout->Controls->Add(
                this->headerPanel,
                0, 0
            );

            this->mainLayout->Controls->Add(
                this->classGroup,
                0, 1
            );

            this->mainLayout->Controls->Add(
                this->streamGroup,
                0, 2
            );


            this->Controls->Add(
                this->mainLayout
            );


            this->ResumeLayout(false);
        }

#pragma endregion


        // =========================================================
        // LOAD ACADEMIC YEARS
        // =========================================================

        void LoadAcademicYears()
        {
            try
            {
                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement>
                    levelStmt(
                        con->prepareStatement(
                            "SELECT al.level_code "
                            "FROM classes c "
                            "INNER JOIN academic_levels al "
                            "ON al.academic_level_id = c.academic_level_id "
                            "WHERE c.class_id = ? "
                            "LIMIT 1"
                        )
                    );

                levelStmt->setInt(1, classId);

                std::unique_ptr<sql::ResultSet> levelResult(
                    levelStmt->executeQuery()
                );

                if (!levelResult->next())
                {
                    MessageBox::Show(
                        L"The selected class does not have a valid academic level.",
                        L"Validation",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Warning
                    );
                    return;
                }

                String^ levelCode =
                    gcnew String(levelResult->getString("level_code").c_str());

                String^ canonicalStream = nullptr;

                if (String::Equals(levelCode, L"O_LEVEL", StringComparison::OrdinalIgnoreCase))
                {
                    if (String::Equals(streamName, L"West", StringComparison::OrdinalIgnoreCase))
                        canonicalStream = L"West";
                    else if (String::Equals(streamName, L"South", StringComparison::OrdinalIgnoreCase))
                        canonicalStream = L"South";
                    else if (String::Equals(streamName, L"East", StringComparison::OrdinalIgnoreCase))
                        canonicalStream = L"East";
                    else if (String::Equals(streamName, L"North", StringComparison::OrdinalIgnoreCase))
                        canonicalStream = L"North";
                }
                else if (String::Equals(levelCode, L"A_LEVEL", StringComparison::OrdinalIgnoreCase))
                {
                    if (String::Equals(streamName, L"Sciences", StringComparison::OrdinalIgnoreCase))
                        canonicalStream = L"Sciences";
                    else if (String::Equals(streamName, L"Arts", StringComparison::OrdinalIgnoreCase))
                        canonicalStream = L"Arts";
                }

                if (canonicalStream == nullptr)
                {
                    MessageBox::Show(
                        String::Equals(levelCode, L"O_LEVEL", StringComparison::OrdinalIgnoreCase)
                        ? L"O-Level streams must be West, South, East or North."
                        : L"A-Level streams must be Sciences or Arts.",
                        L"Validation",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Warning
                    );
                    this->txtStreamName->Focus();
                    return;
                }

                streamName = canonicalStream;


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
                    stmt->executeQuery()
                );


                this->cmbAcademicYear->Items->Clear();

                this->cmbAcademicYear->Items->Add(
                    L"Select Academic Year"
                );


                while (result->next())
                {
                    this->cmbAcademicYear->Items->Add(
                        gcnew ComboItem(
                            result->getInt(
                                "academic_year_id"
                            ),
                            gcnew String(
                                result->getString(
                                    "year_name"
                                ).c_str()
                            )
                        )
                    );
                }


                this->cmbAcademicYear->SelectedIndex =
                    0;
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


        // =========================================================
        // LOAD TERMS
        // =========================================================

        void LoadTerms()
        {
            this->cmbTerm->Items->Clear();

            this->cmbTerm->Items->Add(
                L"Select Term"
            );

            this->cmbTerm->SelectedIndex =
                0;

            this->cmbTerm->Enabled =
                false;


            if (
                this->cmbAcademicYear->SelectedIndex <= 0
                )
            {
                return;
            }


            ComboItem^ yearItem =
                safe_cast<ComboItem^>(
                    this->cmbAcademicYear
                    ->SelectedItem
                    );


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
                        "WHERE academic_year_id = ? "
                        "AND status = 'Active' "
                        "ORDER BY term_id"
                    )
                );


                stmt->setInt(
                    1,
                    yearItem->Id
                );


                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );


                while (result->next())
                {
                    this->cmbTerm->Items->Add(
                        gcnew ComboItem(
                            result->getInt(
                                "term_id"
                            ),
                            gcnew String(
                                result->getString(
                                    "term_name"
                                ).c_str()
                            )
                        )
                    );
                }


                this->cmbTerm->Enabled =
                    this->cmbTerm->Items->Count > 1;
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


        // =========================================================
        // LOAD CLASSES
        // =========================================================

        void LoadClasses()
        {
            try
            {
                auto con =
                    DbConnection::GetConnection();


                String^ yearName =
                    L"";


                String^ termName =
                    L"";


                ComboItem^ yearItem = nullptr;
                ComboItem^ termItem = nullptr;


                if (
                    this->cmbAcademicYear->SelectedIndex > 0
                    )
                {
                    yearItem =
                        safe_cast<ComboItem^>(
                            this->cmbAcademicYear
                            ->SelectedItem
                            );

                    yearName =
                        yearItem->Text;
                }


                if (
                    this->cmbTerm->SelectedIndex > 0
                    )
                {
                    termItem =
                        safe_cast<ComboItem^>(
                            this->cmbTerm
                            ->SelectedItem
                            );

                    termName =
                        termItem->Text;
                }


                std::unique_ptr<sql::PreparedStatement> stmt;


                if (
                    yearItem != nullptr &&
                    termItem != nullptr
                    )
                {
                    stmt.reset(
                        con->prepareStatement(
                            "SELECT "
                            "c.class_id, "
                            "c.class_name, "
                            "al.level_name, "
                            "c.status, "
                            "COUNT(DISTINCT e.student_id) "
                            "AS student_count "
                            "FROM classes c "
                            "INNER JOIN academic_levels al "
                            "ON al.academic_level_id = c.academic_level_id "
                            "LEFT JOIN enrollments e "
                            "ON e.class_id = c.class_id "
                            "AND e.academic_year_id = ? "
                            "AND e.term_id = ? "
                            "AND e.status = 'Active' "
                            "GROUP BY "
                            "c.class_id, "
                            "c.class_name, "
                            "al.level_name, "
                            "c.status "
                            "ORDER BY c.class_name"
                        )
                    );


                    stmt->setInt(
                        1,
                        yearItem->Id
                    );

                    stmt->setInt(
                        2,
                        termItem->Id
                    );
                }
                else
                {
                    stmt.reset(
                        con->prepareStatement(
                            "SELECT "
                            "c.class_id, "
                            "c.class_name, "
                            "c.status, "
                            "0 AS student_count "
                            "FROM classes c "
                            "ORDER BY c.class_name"
                        )
                    );
                }


                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );


                this->classesGrid->Rows->Clear();


                while (result->next())
                {
                    this->classesGrid->Rows->Add(
                        result->getInt(
                            "class_id"
                        ),
                        gcnew String(
                            result->getString(
                                "class_name"
                            ).c_str()
                        ),
                        gcnew String(
                            result->getString(
                                "level_name"
                            ).c_str()
                        ),
                        result->getInt(
                            "student_count"
                        ),
                        gcnew String(
                            result->getString(
                                "status"
                            ).c_str()
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


        // =========================================================
        // LOAD STREAMS
        // =========================================================

        void LoadStreams()
        {
            this->streamsGrid->Rows->Clear();


            if (
                this->classesGrid->SelectedRows->Count == 0
                )
            {
                this->lblSelectedClassValue->Text =
                    L"No class selected.";

                this->btnAddStream->Enabled =
                    false;

                this->btnToggleStream->Enabled =
                    false;

                return;
            }


            if (
                this->cmbAcademicYear->SelectedIndex <= 0 ||
                this->cmbTerm->SelectedIndex <= 0
                )
            {
                this->btnAddStream->Enabled =
                    false;

                this->btnToggleStream->Enabled =
                    false;

                return;
            }


            DataGridViewRow^ classRow =
                this->classesGrid
                ->SelectedRows[0];


            int classId =
                Convert::ToInt32(
                    classRow->Cells["id"]->Value
                );


            ComboItem^ yearItem =
                safe_cast<ComboItem^>(
                    this->cmbAcademicYear
                    ->SelectedItem
                    );


            ComboItem^ termItem =
                safe_cast<ComboItem^>(
                    this->cmbTerm
                    ->SelectedItem
                    );


            this->lblSelectedClassValue->Text =
                Convert::ToString(
                    classRow->Cells["class_name"]->Value
                );


            try
            {
                auto con =
                    DbConnection::GetConnection();


                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT "
                        "s.stream_id, "
                        "s.stream_name, "
                        "s.status, "
                        "COUNT(DISTINCT e.student_id) "
                        "AS student_count "
                        "FROM streams s "
                        "LEFT JOIN enrollments e "
                        "ON e.stream_id = s.stream_id "
                        "AND e.academic_year_id = ? "
                        "AND e.term_id = ? "
                        "AND e.status = 'Active' "
                        "WHERE s.class_id = ? "
                        "GROUP BY "
                        "s.stream_id, "
                        "s.stream_name, "
                        "s.status "
                        "ORDER BY s.stream_name"
                    )
                );


                stmt->setInt(
                    1,
                    yearItem->Id
                );

                stmt->setInt(
                    2,
                    termItem->Id
                );

                stmt->setInt(
                    3,
                    classId
                );


                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );


                while (result->next())
                {
                    this->streamsGrid->Rows->Add(
                        result->getInt(
                            "stream_id"
                        ),
                        gcnew String(
                            result->getString(
                                "stream_name"
                            ).c_str()
                        ),
                        result->getInt(
                            "student_count"
                        ),
                        gcnew String(
                            result->getString(
                                "status"
                            ).c_str()
                        )
                    );
                }


                this->btnAddStream->Enabled =
                    true;

                this->btnToggleStream->Enabled =
                    this->streamsGrid
                    ->SelectedRows->Count > 0;
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


        // =========================================================
        // ADD CLASS
        // =========================================================

        System::Void btnAddClass_Click(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            String^ className =
                this->cmbClassName->SelectedItem->ToString();


            if (
                String::IsNullOrWhiteSpace(
                    className
                )
                )
            {
                MessageBox::Show(
                    L"Please enter a class name.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                this->cmbClassName->Focus();

                return;
            }


            try
            {
                auto con =
                    DbConnection::GetConnection();


                std::unique_ptr<sql::PreparedStatement>
                    checkStmt(
                        con->prepareStatement(
                            "SELECT class_id "
                            "FROM classes "
                            "WHERE class_name = ?"
                        )
                    );


                checkStmt->setString(
                    1,
                    msclr::interop::marshal_as<std::string>(
                        className
                    )
                );


                std::unique_ptr<sql::ResultSet>
                    checkResult(
                        checkStmt->executeQuery()
                    );


                if (checkResult->next())
                {
                    MessageBox::Show(
                        L"That class already exists.",
                        L"Validation",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Warning
                    );

                    return;
                }


                std::unique_ptr<sql::PreparedStatement>
                    levelStmt(
                        con->prepareStatement(
                            "SELECT academic_level_id "
                            "FROM academic_levels "
                            "WHERE level_code = CASE "
                            "WHEN ? IN ('Senior 1','Senior 2','Senior 3','Senior 4') "
                            "THEN 'O_LEVEL' "
                            "WHEN ? IN ('Senior 5','Senior 6') "
                            "THEN 'A_LEVEL' "
                            "ELSE NULL END "
                            "LIMIT 1"
                        )
                    );

                std::string classNameStd =
                    msclr::interop::marshal_as<std::string>(className);

                levelStmt->setString(1, classNameStd);
                levelStmt->setString(2, classNameStd);

                std::unique_ptr<sql::ResultSet> levelResult(
                    levelStmt->executeQuery()
                );

                if (!levelResult->next())
                {
                    MessageBox::Show(
                        L"Only Senior 1 to Senior 6 can be configured as secondary-school classes.",
                        L"Validation",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Warning
                    );
                    return;
                }

                int academicLevelId =
                    levelResult->getInt("academic_level_id");

                std::unique_ptr<sql::PreparedStatement>
                    insertStmt(
                        con->prepareStatement(
                            "INSERT INTO classes "
                            "(class_name, academic_level_id, status) "
                            "VALUES (?, ?, 'Active')"
                        )
                    );

                insertStmt->setString(1, classNameStd);
                insertStmt->setInt(2, academicLevelId);

                insertStmt->executeUpdate();


                this->cmbClassName->SelectedIndex = 0;

                LoadClasses();


                MessageBox::Show(
                    L"Class added successfully.",
                    L"Classes",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information
                );
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


        // =========================================================
        // CLASS SELECTED
        // =========================================================

        System::Void classesGrid_SelectionChanged(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            LoadStreams();
        }


        // =========================================================
        // ADD STREAM
        // =========================================================

        System::Void btnAddStream_Click(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            if (
                this->classesGrid->SelectedRows->Count == 0
                )
            {
                MessageBox::Show(
                    L"Please select a class first.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                return;
            }


            if (
                this->cmbAcademicYear->SelectedIndex <= 0
                )
            {
                MessageBox::Show(
                    L"Please select an academic year.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                return;
            }


            if (
                this->cmbTerm->SelectedIndex <= 0
                )
            {
                MessageBox::Show(
                    L"Please select a term.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                return;
            }


            String^ streamName =
                this->txtStreamName->Text->Trim();


            if (
                String::IsNullOrWhiteSpace(
                    streamName
                )
                )
            {
                MessageBox::Show(
                    L"Please enter a stream name.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                this->txtStreamName->Focus();

                return;
            }


            int classId =
                Convert::ToInt32(
                    this->classesGrid
                    ->SelectedRows[0]
                    ->Cells["id"]
                    ->Value
                );


            try
            {
                auto con =
                    DbConnection::GetConnection();


                std::unique_ptr<sql::PreparedStatement>
                    checkStmt(
                        con->prepareStatement(
                            "SELECT stream_id "
                            "FROM streams "
                            "WHERE class_id = ? "
                            "AND stream_name = ?"
                        )
                    );


                checkStmt->setInt(
                    1,
                    classId
                );


                checkStmt->setString(
                    2,
                    msclr::interop::marshal_as<std::string>(
                        streamName
                    )
                );


                std::unique_ptr<sql::ResultSet>
                    checkResult(
                        checkStmt->executeQuery()
                    );


                if (checkResult->next())
                {
                    MessageBox::Show(
                        L"That stream already exists in this class.",
                        L"Validation",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Warning
                    );

                    return;
                }


                std::unique_ptr<sql::PreparedStatement>
                    insertStmt(
                        con->prepareStatement(
                            "INSERT INTO streams "
                            "(class_id, stream_name, status) "
                            "VALUES (?, ?, 'Active')"
                        )
                    );


                insertStmt->setInt(
                    1,
                    classId
                );


                insertStmt->setString(
                    2,
                    msclr::interop::marshal_as<std::string>(
                        streamName
                    )
                );


                insertStmt->executeUpdate();


                this->txtStreamName->Clear();


                LoadStreams();


                MessageBox::Show(
                    L"Stream added successfully.",
                    L"Streams",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information
                );
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


        // =========================================================
        // ACADEMIC YEAR CHANGED
        // =========================================================

        System::Void cmbAcademicYear_SelectedIndexChanged(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            LoadTerms();
            LoadClasses();
        }


        // =========================================================
        // TERM CHANGED
        // =========================================================

        System::Void cmbTerm_SelectedIndexChanged(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            LoadClasses();
        }


        // =========================================================
        // TOGGLE CLASS
        // =========================================================

        System::Void btnToggleClass_Click(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            if (
                this->classesGrid->SelectedRows->Count == 0
                )
            {
                MessageBox::Show(
                    L"Please select a class.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                return;
            }


            DataGridViewRow^ row =
                this->classesGrid->SelectedRows[0];


            int classId =
                Convert::ToInt32(
                    row->Cells["id"]->Value
                );


            String^ currentStatus =
                Convert::ToString(
                    row->Cells["status"]->Value
                );


            String^ newStatus =
                currentStatus->Equals(
                    L"Active",
                    StringComparison::OrdinalIgnoreCase
                )
                ? L"Inactive"
                : L"Active";


            try
            {
                auto con =
                    DbConnection::GetConnection();


                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "UPDATE classes "
                        "SET status = ? "
                        "WHERE class_id = ?"
                    )
                );


                stmt->setString(
                    1,
                    msclr::interop::marshal_as<std::string>(
                        newStatus
                    )
                );

                stmt->setInt(
                    2,
                    classId
                );


                stmt->executeUpdate();


                LoadClasses();

                MessageBox::Show(
                    L"Class status updated.",
                    L"Classes",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information
                );
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


        // =========================================================
        // TOGGLE STREAM
        // =========================================================

        System::Void streamsGrid_SelectionChanged(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            this->btnToggleStream->Enabled =
                this->streamsGrid->SelectedRows->Count > 0;
        }


        System::Void btnToggleStream_Click(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            if (
                this->streamsGrid->SelectedRows->Count == 0
                )
            {
                MessageBox::Show(
                    L"Please select a stream.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                return;
            }


            DataGridViewRow^ row =
                this->streamsGrid->SelectedRows[0];


            int streamId =
                Convert::ToInt32(
                    row->Cells["id"]->Value
                );


            String^ currentStatus =
                Convert::ToString(
                    row->Cells["status"]->Value
                );


            String^ newStatus =
                currentStatus->Equals(
                    L"Active",
                    StringComparison::OrdinalIgnoreCase
                )
                ? L"Inactive"
                : L"Active";


            try
            {
                auto con =
                    DbConnection::GetConnection();


                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "UPDATE streams "
                        "SET status = ? "
                        "WHERE stream_id = ?"
                    )
                );


                stmt->setString(
                    1,
                    msclr::interop::marshal_as<std::string>(
                        newStatus
                    )
                );

                stmt->setInt(
                    2,
                    streamId
                );


                stmt->executeUpdate();


                LoadStreams();


                MessageBox::Show(
                    L"Stream status updated.",
                    L"Streams",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information
                );
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


        // =========================================================
        // BACK
        // =========================================================

        System::Void btnBack_Click(
            System::Object^ sender,
            System::EventArgs^ e)
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

        // =========================================================
        // CONSTRUCTOR
        // =========================================================


    };
}
#pragma once

#include "DbConnection.h"

#include <mariadb/conncpp.hpp>
#include <msclr/marshal_cppstd.h>

#include <memory>
#include <string>

using namespace System;
using namespace System::Drawing;
using namespace System::Windows::Forms;

namespace SchoolCore
{
    public ref class ClassesStreams : public Form
    {
    private:

        // =========================================================
        // CONTROLS
        // =========================================================

        TableLayoutPanel^ mainLayout;

        Panel^ headerPanel;
        Label^ lblTitle;
        Label^ lblSubtitle;

        GroupBox^ classGroup;
        GroupBox^ streamGroup;

        // Classes
        Label^ lblClassName;
        ComboBox^ cmbClassName;
        Button^ btnAddClass;
        Button^ btnToggleClass;
        DataGridView^ classesGrid;

        // Streams
        Label^ lblSelectedClass;
        Label^ lblSelectedClassValue;
        Label^ lblAcademicYear;
        Label^ lblTerm;
        Label^ lblStreamName;

        ComboBox^ cmbAcademicYear;
        ComboBox^ cmbTerm;

        TextBox^ txtStreamName;

        Button^ btnAddStream;
        Button^ btnToggleStream;

        DataGridView^ streamsGrid;

        // Bottom
        Panel^ buttonPanel;


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


        // =========================================================
        // INITIALIZE COMPONENTS
        // =========================================================

        void InitializeComponent()
        {
            this->SuspendLayout();

            // =====================================================
            // FORM
            // =====================================================

            this->Text = L"Classes & Streams";

            this->StartPosition =
                FormStartPosition::CenterParent;

            this->WindowState =
                FormWindowState::Maximized;

            this->MinimumSize =
                System::Drawing::Size(900, 600);

            this->BackColor =
                Color::WhiteSmoke;

            this->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.5F,
                    FontStyle::Regular
                );


            // =====================================================
            // MAIN LAYOUT
            // =====================================================

            this->mainLayout =
                gcnew TableLayoutPanel();

            this->mainLayout->Dock =
                DockStyle::Fill;

            this->mainLayout->ColumnCount = 1;
            this->mainLayout->RowCount = 3;

            this->mainLayout->Padding =
                System::Windows::Forms::Padding(20);

            this->mainLayout->BackColor =
                Color::WhiteSmoke;


            this->mainLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    100.0F
                )
            );

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    78.0F
                )
            );

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Percent,
                    50.0F
                )
            );

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Percent,
                    50.0F
                )
            );

          


            // =====================================================
            // HEADER
            // =====================================================

            this->headerPanel =
                gcnew Panel();

            this->headerPanel->Dock =
                DockStyle::Fill;

            this->headerPanel->BackColor =
                Color::FromArgb(
                    35, 47, 62
                );

            this->headerPanel->Padding =
                System::Windows::Forms::Padding(
                    20, 10, 20, 10
                );


            this->lblTitle =
                gcnew Label();

            this->lblTitle->AutoSize = true;

            this->lblTitle->Text =
                L"Classes & Streams";

            this->lblTitle->ForeColor =
                Color::White;

            this->lblTitle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    18.0F,
                    FontStyle::Bold
                );

            this->lblTitle->Location =
                Point(18, 10);


            this->lblSubtitle =
                gcnew Label();

            this->lblSubtitle->AutoSize = true;

            this->lblSubtitle->Text =
                L"Manage the Senior 1–Senior 6 classes and monitor live student enrollment.";

            this->lblSubtitle->ForeColor =
                Color::FromArgb(
                    220, 225, 230
                );

            this->lblSubtitle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.5F,
                    FontStyle::Regular
                );

            this->lblSubtitle->Location =
                Point(20, 45);


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
                gcnew GroupBox();

            this->classGroup->Dock =
                DockStyle::Fill;

            this->classGroup->Text =
                L"Classes";

            this->classGroup->Padding =
                System::Windows::Forms::Padding(12);

            this->classGroup->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    10.0F,
                    FontStyle::Bold
                );


            TableLayoutPanel^ classLayout =
                gcnew TableLayoutPanel();

            classLayout->Dock =
                DockStyle::Fill;

            classLayout->ColumnCount = 4;
            classLayout->RowCount = 2;

            classLayout->Padding =
                System::Windows::Forms::Padding(5);


            classLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Absolute,
                    110.0F
                )
            );

            classLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    55.0F
                )
            );

            classLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Absolute,
                    150.0F
                )
            );

            classLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    45.0F
                )
            );


            classLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    45.0F
                )
            );

            classLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Percent,
                    100.0F
                )
            );


            // Class label

            this->lblClassName =
                gcnew Label();

            this->lblClassName->Text =
                L"Class Name";

            this->lblClassName->Dock =
                DockStyle::Fill;

            this->lblClassName->TextAlign =
                ContentAlignment::MiddleLeft;


            // Class input

            this->cmbClassName =
                gcnew ComboBox();

            this->cmbClassName->Dock =
                DockStyle::Fill;

            this->cmbClassName->DropDownStyle =
                ComboBoxStyle::DropDownList;

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
                gcnew Button();

            this->btnAddClass->Text =
                L"+ Add Class";

            this->btnAddClass->Dock =
                DockStyle::Fill;


            // Toggle class

            this->btnToggleClass =
                gcnew Button();

            this->btnToggleClass->Text =
                L"Activate / Deactivate";

            this->btnToggleClass->Dock =
                DockStyle::Fill;


            // Classes grid

            this->classesGrid =
                gcnew DataGridView();

            this->classesGrid->Dock =
                DockStyle::Fill;

            this->classesGrid->AllowUserToAddRows =
                false;

            this->classesGrid->AllowUserToDeleteRows =
                false;

            this->classesGrid->ReadOnly =
                true;

            this->classesGrid->SelectionMode =
                DataGridViewSelectionMode::FullRowSelect;

            this->classesGrid->MultiSelect =
                false;

            this->classesGrid->AutoSizeColumnsMode =
                DataGridViewAutoSizeColumnsMode::Fill;

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
                L"student_count",
                L"Students"
            );

            this->classesGrid->Columns->Add(
                L"status",
                L"Status"
            );


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
                gcnew GroupBox();

            this->streamGroup->Dock =
                DockStyle::Fill;

            this->streamGroup->Text =
                L"Streams";

            this->streamGroup->Padding =
                System::Windows::Forms::Padding(12);

            this->streamGroup->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    10.0F,
                    FontStyle::Bold
                );


            TableLayoutPanel^ streamLayout =
                gcnew TableLayoutPanel();

            streamLayout->Dock =
                DockStyle::Fill;

            streamLayout->ColumnCount = 6;
            streamLayout->RowCount = 3;

            streamLayout->Padding =
                System::Windows::Forms::Padding(5);


            streamLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Absolute,
                    105.0F
                )
            );

            streamLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    30.0F
                )
            );

            streamLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Absolute,
                    105.0F
                )
            );

            streamLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    25.0F
                )
            );

            streamLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Absolute,
                    90.0F
                )
            );

            streamLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    45.0F
                )
            );


            streamLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    40.0F
                )
            );

            streamLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    40.0F
                )
            );

            streamLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Percent,
                    100.0F
                )
            );


            // Selected class

            this->lblSelectedClass =
                gcnew Label();

            this->lblSelectedClass->Text =
                L"Class";

            this->lblSelectedClass->Dock =
                DockStyle::Fill;

            this->lblSelectedClass->TextAlign =
                ContentAlignment::MiddleLeft;


            this->lblSelectedClassValue =
                gcnew Label();

            this->lblSelectedClassValue->Text =
                L"No class selected.";

            this->lblSelectedClassValue->Dock =
                DockStyle::Fill;

            this->lblSelectedClassValue->TextAlign =
                ContentAlignment::MiddleLeft;

            this->lblSelectedClassValue->ForeColor =
                Color::DimGray;


            // Academic year

            this->lblAcademicYear =
                gcnew Label();

            this->lblAcademicYear->Text =
                L"Academic Year";

            this->lblAcademicYear->Dock =
                DockStyle::Fill;

            this->lblAcademicYear->TextAlign =
                ContentAlignment::MiddleLeft;


            this->cmbAcademicYear =
                gcnew ComboBox();

            this->cmbAcademicYear->Dock =
                DockStyle::Fill;

            this->cmbAcademicYear->DropDownStyle =
                ComboBoxStyle::DropDownList;


            // Term

            this->lblTerm =
                gcnew Label();

            this->lblTerm->Text =
                L"Term";

            this->lblTerm->Dock =
                DockStyle::Fill;

            this->lblTerm->TextAlign =
                ContentAlignment::MiddleLeft;


            this->cmbTerm =
                gcnew ComboBox();

            this->cmbTerm->Dock =
                DockStyle::Fill;

            this->cmbTerm->DropDownStyle =
                ComboBoxStyle::DropDownList;

            this->cmbTerm->Enabled =
                false;


            // Stream name

            this->lblStreamName =
                gcnew Label();

            this->lblStreamName->Text =
                L"Stream";

            this->lblStreamName->Dock =
                DockStyle::Fill;

            this->lblStreamName->TextAlign =
                ContentAlignment::MiddleLeft;


            this->txtStreamName =
                gcnew TextBox();

            this->txtStreamName->Dock =
                DockStyle::Fill;

            this->txtStreamName->Margin =
                System::Windows::Forms::Padding(3);


            // Add stream

            this->btnAddStream =
                gcnew Button();

            this->btnAddStream->Text =
                L"+ Add Stream";

            this->btnAddStream->Dock =
                DockStyle::Fill;

            this->btnAddStream->Enabled =
                false;


            // Toggle stream

            this->btnToggleStream =
                gcnew Button();

            this->btnToggleStream->Text =
                L"Activate / Deactivate";

            this->btnToggleStream->Dock =
                DockStyle::Fill;

            this->btnToggleStream->Enabled =
                false;


            // Stream grid

            this->streamsGrid =
                gcnew DataGridView();

            this->streamsGrid->Dock =
                DockStyle::Fill;

            this->streamsGrid->AllowUserToAddRows =
                false;

            this->streamsGrid->AllowUserToDeleteRows =
                false;

            this->streamsGrid->ReadOnly =
                true;

            this->streamsGrid->SelectionMode =
                DataGridViewSelectionMode::FullRowSelect;

            this->streamsGrid->MultiSelect =
                false;

            this->streamsGrid->AutoSizeColumnsMode =
                DataGridViewAutoSizeColumnsMode::Fill;

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


        // =========================================================
        // LOAD ACADEMIC YEARS
        // =========================================================

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
                            "c.status, "
                            "COUNT(DISTINCT e.student_id) "
                            "AS student_count "
                            "FROM classes c "
                            "LEFT JOIN enrollments e "
                            "ON e.class_id = c.class_id "
                            "AND e.academic_year_id = ? "
                            "AND e.term_id = ? "
                            "AND e.status = 'Active' "
                            "GROUP BY "
                            "c.class_id, "
                            "c.class_name, "
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
                    insertStmt(
                        con->prepareStatement(
                            "INSERT INTO classes "
                            "(class_name, status) "
                            "VALUES (?, 'Active')"
                        )
                    );


                insertStmt->setString(
                    1,
                    msclr::interop::marshal_as<std::string>(
                        className
                    )
                );


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

        ClassesStreams()
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

       


            LoadAcademicYears();
            LoadClasses();
        }
    };
}
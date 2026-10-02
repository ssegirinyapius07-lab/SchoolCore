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
    public ref class AcademicYearsTerms : public Form
    {
    private:

        // =========================================================
        // MAIN CONTROLS
        // =========================================================

        TableLayoutPanel^ mainLayout;

        Panel^ headerPanel;
        Label^ lblTitle;
        Label^ lblSubtitle;

        GroupBox^ yearGroup;
        GroupBox^ termGroup;

        // Academic year
        Label^ lblYearName;
        NumericUpDown^ numAcademicYear;

        Button^ btnAddYear;
        Button^ btnToggleYear;

        DataGridView^ yearsGrid;

        // Terms
        Label^ lblSelectedYear;
        Label^ lblSelectedYearValue;
        Label^ lblTermNote;

        Button^ btnToggleTerm;

        DataGridView^ termsGrid;

        // Bottom
        Panel^ buttonPanel;


        // =========================================================
        // INITIALIZE COMPONENTS
        // =========================================================

        void InitializeComponent()
        {
            this->SuspendLayout();

            // =====================================================
            // FORM
            // =====================================================

            this->Text = L"Academic Years & Terms";

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
                    48.0F
                )
            );

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Percent,
                    48.0F
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
                L"Academic Years & Terms";

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
                L"Create academic years. Each year automatically contains Term 1, Term 2 and Term 3.";

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
            // ACADEMIC YEAR GROUP
            // =====================================================

            this->yearGroup =
                gcnew GroupBox();

            this->yearGroup->Dock =
                DockStyle::Fill;

            this->yearGroup->Text =
                L"Academic Years";

            this->yearGroup->Padding =
                System::Windows::Forms::Padding(12);

            this->yearGroup->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    10.0F,
                    FontStyle::Bold
                );


            TableLayoutPanel^ yearLayout =
                gcnew TableLayoutPanel();

            yearLayout->Dock =
                DockStyle::Fill;

            yearLayout->ColumnCount = 4;
            yearLayout->RowCount = 2;

            yearLayout->Padding =
                System::Windows::Forms::Padding(5);


            yearLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Absolute,
                    120.0F
                )
            );

            yearLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    55.0F
                )
            );

            yearLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Absolute,
                    160.0F
                )
            );

            yearLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    45.0F
                )
            );


            yearLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    45.0F
                )
            );

            yearLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Percent,
                    100.0F
                )
            );


            // Year label

            this->lblYearName =
                gcnew Label();

            this->lblYearName->Text =
                L"Academic Year";

            this->lblYearName->Dock =
                DockStyle::Fill;

            this->lblYearName->TextAlign =
                ContentAlignment::MiddleLeft;


            // Year numeric input

            this->numAcademicYear =
                gcnew NumericUpDown();

            this->numAcademicYear->Dock =
                DockStyle::Left;

            this->numAcademicYear->Width =
                140;

            this->numAcademicYear->Minimum =
                1900;

            this->numAcademicYear->Maximum =
                2100;

            this->numAcademicYear->DecimalPlaces =
                0;

            this->numAcademicYear->ThousandsSeparator =
                false;

            this->numAcademicYear->Value =
                DateTime::Today.Year;

            this->numAcademicYear->Margin =
                System::Windows::Forms::Padding(3);


            // Add Year

            this->btnAddYear =
                gcnew Button();

            this->btnAddYear->Text =
                L"+ Add Academic Year";

            this->btnAddYear->Dock =
                DockStyle::Fill;


            // Toggle Year

            this->btnToggleYear =
                gcnew Button();

            this->btnToggleYear->Text =
                L"Activate / Deactivate";

            this->btnToggleYear->Dock =
                DockStyle::Fill;


            // Years grid

            this->yearsGrid =
                gcnew DataGridView();

            this->yearsGrid->Dock =
                DockStyle::Fill;

            this->yearsGrid->AllowUserToAddRows =
                false;

            this->yearsGrid->AllowUserToDeleteRows =
                false;

            this->yearsGrid->ReadOnly =
                true;

            this->yearsGrid->SelectionMode =
                DataGridViewSelectionMode::FullRowSelect;

            this->yearsGrid->MultiSelect =
                false;

            this->yearsGrid->AutoSizeColumnsMode =
                DataGridViewAutoSizeColumnsMode::Fill;

            this->yearsGrid->RowHeadersVisible =
                false;


            this->yearsGrid->Columns->Add(
                L"id",
                L"ID"
            );

            this->yearsGrid->Columns->Add(
                L"year_name",
                L"Academic Year"
            );

            this->yearsGrid->Columns->Add(
                L"status",
                L"Status"
            );


            yearLayout->Controls->Add(
                this->lblYearName,
                0, 0
            );

            yearLayout->Controls->Add(
                this->numAcademicYear,
                1, 0
            );

            yearLayout->Controls->Add(
                this->btnAddYear,
                2, 0
            );

            yearLayout->Controls->Add(
                this->btnToggleYear,
                3, 0
            );


            yearLayout->Controls->Add(
                this->yearsGrid,
                0, 1
            );

            yearLayout->SetColumnSpan(
                this->yearsGrid,
                4
            );


            this->yearGroup->Controls->Add(
                yearLayout
            );


            // =====================================================
            // TERM GROUP
            // =====================================================

            this->termGroup =
                gcnew GroupBox();

            this->termGroup->Dock =
                DockStyle::Fill;

            this->termGroup->Text =
                L"Terms";

            this->termGroup->Padding =
                System::Windows::Forms::Padding(12);

            this->termGroup->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    10.0F,
                    FontStyle::Bold
                );


            TableLayoutPanel^ termLayout =
                gcnew TableLayoutPanel();

            termLayout->Dock =
                DockStyle::Fill;

            termLayout->ColumnCount = 4;
            termLayout->RowCount = 3;

            termLayout->Padding =
                System::Windows::Forms::Padding(5);


            termLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Absolute,
                    120.0F
                )
            );

            termLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    55.0F
                )
            );

            termLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Absolute,
                    150.0F
                )
            );

            termLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    45.0F
                )
            );


            termLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    40.0F
                )
            );

            termLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    35.0F
                )
            );

            termLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Percent,
                    100.0F
                )
            );


            // Selected year

            this->lblSelectedYear =
                gcnew Label();

            this->lblSelectedYear->Text =
                L"Selected Year";

            this->lblSelectedYear->Dock =
                DockStyle::Fill;

            this->lblSelectedYear->TextAlign =
                ContentAlignment::MiddleLeft;


            this->lblSelectedYearValue =
                gcnew Label();

            this->lblSelectedYearValue->Dock =
                DockStyle::Fill;

            this->lblSelectedYearValue->Text =
                L"No academic year selected.";

            this->lblSelectedYearValue->TextAlign =
                ContentAlignment::MiddleLeft;

            this->lblSelectedYearValue->ForeColor =
                Color::DimGray;


            // Term note

            this->lblTermNote =
                gcnew Label();

            this->lblTermNote->Dock =
                DockStyle::Fill;

            this->lblTermNote->Text =
                L"Each academic year has exactly three terms.";

            this->lblTermNote->TextAlign =
                ContentAlignment::MiddleLeft;

            this->lblTermNote->ForeColor =
                Color::DimGray;


            this->btnToggleTerm =
                gcnew Button();

            this->btnToggleTerm->Text =
                L"Activate / Deactivate";

            this->btnToggleTerm->Dock =
                DockStyle::Fill;

            this->btnToggleTerm->Enabled =
                false;


            // Terms grid

            this->termsGrid =
                gcnew DataGridView();

            this->termsGrid->Dock =
                DockStyle::Fill;

            this->termsGrid->AllowUserToAddRows =
                false;

            this->termsGrid->AllowUserToDeleteRows =
                false;

            this->termsGrid->ReadOnly =
                true;

            this->termsGrid->SelectionMode =
                DataGridViewSelectionMode::FullRowSelect;

            this->termsGrid->MultiSelect =
                false;

            this->termsGrid->AutoSizeColumnsMode =
                DataGridViewAutoSizeColumnsMode::Fill;

            this->termsGrid->RowHeadersVisible =
                false;


            this->termsGrid->Columns->Add(
                L"id",
                L"ID"
            );

            this->termsGrid->Columns->Add(
                L"term_name",
                L"Term"
            );

            this->termsGrid->Columns->Add(
                L"status",
                L"Status"
            );


            termLayout->Controls->Add(
                this->lblSelectedYear,
                0, 0
            );

            termLayout->Controls->Add(
                this->lblSelectedYearValue,
                1, 0
            );

            termLayout->Controls->Add(
                this->lblTermNote,
                2, 0
            );

            termLayout->Controls->Add(
                this->btnToggleTerm,
                3, 0
            );


            termLayout->Controls->Add(
                this->termsGrid,
                0, 2
            );

            termLayout->SetColumnSpan(
                this->termsGrid,
                4
            );


            this->termGroup->Controls->Add(
                termLayout
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
                this->yearGroup,
                0, 1
            );

            this->mainLayout->Controls->Add(
                this->termGroup,
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
                        "year_name, "
                        "status "
                        "FROM academic_years "
                        "ORDER BY academic_year_id DESC"
                    )
                );


                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );


                this->yearsGrid->Rows->Clear();


                while (result->next())
                {
                    this->yearsGrid->Rows->Add(
                        result->getInt(
                            "academic_year_id"
                        ),
                        gcnew String(
                            result->getString(
                                "year_name"
                            ).c_str()
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
        // LOAD TERMS FOR SELECTED YEAR
        // =========================================================

        void LoadTermsForYear(
            int academicYearId)
        {
            try
            {
                auto con =
                    DbConnection::GetConnection();


                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT "
                        "term_id, "
                        "term_name, "
                        "status "
                        "FROM terms "
                        "WHERE academic_year_id = ? "
                        "ORDER BY term_id"
                    )
                );


                stmt->setInt(
                    1,
                    academicYearId
                );


                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );


                this->termsGrid->Rows->Clear();


                while (result->next())
                {
                    this->termsGrid->Rows->Add(
                        result->getInt(
                            "term_id"
                        ),
                        gcnew String(
                            result->getString(
                                "term_name"
                            ).c_str()
                        ),
                        gcnew String(
                            result->getString(
                                "status"
                            ).c_str()
                        )
                    );
                }


                this->btnToggleTerm->Enabled =
                    this->termsGrid->Rows->Count > 0;
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
        // ADD ACADEMIC YEAR
        // =========================================================

        System::Void btnAddYear_Click(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            int year =
                Decimal::ToInt32(
                    this->numAcademicYear->Value
                );


            String^ yearName =
                year.ToString();


            try
            {
                auto con =
                    DbConnection::GetConnection();


                // -----------------------------------------------
                // Check whether year already exists
                // -----------------------------------------------

                std::unique_ptr<sql::PreparedStatement>
                    checkStmt(
                        con->prepareStatement(
                            "SELECT academic_year_id "
                            "FROM academic_years "
                            "WHERE year_name = ?"
                        )
                    );


                checkStmt->setString(
                    1,
                    msclr::interop::marshal_as<std::string>(
                        yearName
                    )
                );


                std::unique_ptr<sql::ResultSet>
                    checkResult(
                        checkStmt->executeQuery()
                    );


                if (checkResult->next())
                {
                    MessageBox::Show(
                        L"That academic year already exists.",
                        L"Academic Years",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Warning
                    );

                    return;
                }


                // -----------------------------------------------
                // Transaction
                // -----------------------------------------------

                con->setAutoCommit(false);


                // -----------------------------------------------
                // Insert academic year
                // -----------------------------------------------

                std::unique_ptr<sql::PreparedStatement>
                    insertYear(
                        con->prepareStatement(
                            "INSERT INTO academic_years "
                            "(year_name, status) "
                            "VALUES (?, 'Active')"
                        )
                    );


                insertYear->setString(
                    1,
                    msclr::interop::marshal_as<std::string>(
                        yearName
                    )
                );


                insertYear->executeUpdate();


                // -----------------------------------------------
                // Get inserted year ID
                // -----------------------------------------------

                std::unique_ptr<sql::PreparedStatement>
                    idStmt(
                        con->prepareStatement(
                            "SELECT LAST_INSERT_ID() "
                            "AS academic_year_id"
                        )
                    );


                std::unique_ptr<sql::ResultSet>
                    idResult(
                        idStmt->executeQuery()
                    );


                if (!idResult->next())
                {
                    throw std::runtime_error(
                        "Unable to retrieve academic year ID."
                    );
                }


                int academicYearId =
                    idResult->getInt(
                        "academic_year_id"
                    );


                // -----------------------------------------------
                // Automatically create exactly 3 terms
                // -----------------------------------------------

                std::unique_ptr<sql::PreparedStatement>
                    insertTerm(
                        con->prepareStatement(
                            "INSERT INTO terms "
                            "(academic_year_id, term_name, status) "
                            "VALUES (?, ?, 'Active')"
                        )
                    );


                insertTerm->setInt(
                    1,
                    academicYearId
                );


                insertTerm->setString(
                    2,
                    "Term 1"
                );

                insertTerm->executeUpdate();


                insertTerm->setInt(
                    1,
                    academicYearId
                );

                insertTerm->setString(
                    2,
                    "Term 2"
                );

                insertTerm->executeUpdate();


                insertTerm->setInt(
                    1,
                    academicYearId
                );

                insertTerm->setString(
                    2,
                    "Term 3"
                );

                insertTerm->executeUpdate();


                // -----------------------------------------------
                // Commit
                // -----------------------------------------------

                con->commit();


                this->numAcademicYear->Value =
                    Math::Min(
                        2100,
                        year + 1
                    );


                LoadAcademicYears();


                MessageBox::Show(
                    L"Academic year " +
                    yearName +
                    L" was created with Term 1, Term 2 and Term 3.",
                    L"Academic Years",
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
            catch (std::exception& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }


        // =========================================================
        // YEAR SELECTION
        // =========================================================

        System::Void yearsGrid_SelectionChanged(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            if (
                this->yearsGrid->SelectedRows->Count == 0
                )
            {
                this->lblSelectedYearValue->Text =
                    L"No academic year selected.";

                this->btnToggleTerm->Enabled =
                    false;

                this->termsGrid->Rows->Clear();

                return;
            }


            DataGridViewRow^ row =
                this->yearsGrid->SelectedRows[0];


            int academicYearId =
                Convert::ToInt32(
                    row->Cells["id"]->Value
                );


            String^ yearName =
                Convert::ToString(
                    row->Cells["year_name"]->Value
                );


            this->lblSelectedYearValue->Text =
                yearName;


            LoadTermsForYear(
                academicYearId
            );
        }


        // =========================================================
        // TOGGLE ACADEMIC YEAR
        // =========================================================

        System::Void btnToggleYear_Click(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            if (
                this->yearsGrid->SelectedRows->Count == 0
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


            DataGridViewRow^ row =
                this->yearsGrid->SelectedRows[0];


            int academicYearId =
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
                        "UPDATE academic_years "
                        "SET status = ? "
                        "WHERE academic_year_id = ?"
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
                    academicYearId
                );


                stmt->executeUpdate();


                LoadAcademicYears();


                MessageBox::Show(
                    L"Academic year status updated.",
                    L"Academic Years",
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
        // TOGGLE TERM
        // =========================================================

        System::Void btnToggleTerm_Click(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            if (
                this->termsGrid->SelectedRows->Count == 0
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


            DataGridViewRow^ row =
                this->termsGrid->SelectedRows[0];


            int termId =
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
                        "UPDATE terms "
                        "SET status = ? "
                        "WHERE term_id = ?"
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
                    termId
                );


                stmt->executeUpdate();


                if (
                    this->yearsGrid->SelectedRows->Count > 0
                    )
                {
                    int academicYearId =
                        Convert::ToInt32(
                            this->yearsGrid
                            ->SelectedRows[0]
                            ->Cells["id"]
                            ->Value
                        );


                    LoadTermsForYear(
                        academicYearId
                    );
                }


                MessageBox::Show(
                    L"Term status updated.",
                    L"Terms",
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

       

    public:

        // =========================================================
        // CONSTRUCTOR
        // =========================================================

        AcademicYearsTerms()
        {
            InitializeComponent();


            this->btnAddYear->Click +=
                gcnew System::EventHandler(
                    this,
                    &AcademicYearsTerms::btnAddYear_Click
                );


            this->btnToggleYear->Click +=
                gcnew System::EventHandler(
                    this,
                    &AcademicYearsTerms::btnToggleYear_Click
                );


            this->yearsGrid->SelectionChanged +=
                gcnew System::EventHandler(
                    this,
                    &AcademicYearsTerms::yearsGrid_SelectionChanged
                );


            this->btnToggleTerm->Click +=
                gcnew System::EventHandler(
                    this,
                    &AcademicYearsTerms::btnToggleTerm_Click
                );


           

            LoadAcademicYears();
        }
    };
}
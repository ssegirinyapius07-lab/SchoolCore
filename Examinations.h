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
    public ref class Examinations : public Form
    {
    private:

        ref class FilterItem
        {
        public:
            int Id;
            String^ Text;

            FilterItem(int id, String^ text)
            {
                Id = id;
                Text = text;
            }

            virtual String^ ToString() override
            {
                return Text;
            }
        };


        TableLayoutPanel^ mainLayout;
        Panel^ headerPanel;
        Label^ lblTitle;
        Label^ lblSubtitle;

        Panel^ filterPanel;
        ComboBox^ cmbAcademicYear;
        ComboBox^ cmbTerm;
        ComboBox^ cmbClass;
        ComboBox^ cmbStatus;
        Button^ btnRefresh;

        Label^ lblExamCount;
        DataGridView^ examinationsGrid;

        FlowLayoutPanel^ actionPanel;
        Button^ btnNew;
        Button^ btnEdit;
        Button^ btnSubjects;
        Button^ btnMarks;
        Button^ btnResults;
        Button^ btnBack;

        bool loadingFilters = false;


        int GetSelectedId(ComboBox^ combo)
        {
            if (combo == nullptr ||
                combo->SelectedItem == nullptr)
            {
                return 0;
            }

            FilterItem^ item =
                dynamic_cast<FilterItem^>(
                    combo->SelectedItem
                );

            return item == nullptr ? 0 : item->Id;
        }


        void AddFilterItem(
            ComboBox^ combo,
            int id,
            String^ text)
        {
            combo->Items->Add(
                gcnew FilterItem(
                    id,
                    text
                )
            );
        }


        void LoadAcademicYears()
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

            this->cmbAcademicYear->Items->Clear();
            this->cmbAcademicYear->Items->Add(
                L"All Academic Years"
            );

            while (result->next())
            {
                AddFilterItem(
                    this->cmbAcademicYear,
                    result->getInt(
                        "academic_year_id"
                    ),
                    gcnew String(
                        result->getString(
                            "year_name"
                        ).c_str()
                    )
                );
            }

            this->cmbAcademicYear->SelectedIndex = 0;
        }


        void LoadTerms()
        {
            int yearId =
                GetSelectedId(
                    this->cmbAcademicYear
                );

            this->cmbTerm->Items->Clear();

            this->cmbTerm->Items->Add(
                L"All Terms"
            );

            if (yearId == 0)
            {
                this->cmbTerm->SelectedIndex = 0;
                this->cmbTerm->Enabled = false;
                return;
            }

            auto con = DbConnection::GetConnection();

            std::unique_ptr<sql::PreparedStatement> stmt(
                con->prepareStatement(
                    "SELECT term_id, term_name "
                    "FROM terms "
                    "WHERE academic_year_id = ? "
                    "ORDER BY term_id ASC"
                )
            );

            stmt->setInt(
                1,
                yearId
            );

            std::unique_ptr<sql::ResultSet> result(
                stmt->executeQuery()
            );

            while (result->next())
            {
                AddFilterItem(
                    this->cmbTerm,
                    result->getInt(
                        "term_id"
                    ),
                    gcnew String(
                        result->getString(
                            "term_name"
                        ).c_str()
                    )
                );
            }

            this->cmbTerm->SelectedIndex = 0;
            this->cmbTerm->Enabled = true;
        }


        void LoadClasses()
        {
            auto con = DbConnection::GetConnection();

            std::unique_ptr<sql::PreparedStatement> stmt(
                con->prepareStatement(
                    "SELECT class_id, class_name "
                    "FROM classes "
                    "WHERE status = 'Active' "
                    "ORDER BY class_id ASC"
                )
            );

            std::unique_ptr<sql::ResultSet> result(
                stmt->executeQuery()
            );

            this->cmbClass->Items->Clear();
            this->cmbClass->Items->Add(
                L"All Classes"
            );

            while (result->next())
            {
                AddFilterItem(
                    this->cmbClass,
                    result->getInt(
                        "class_id"
                    ),
                    gcnew String(
                        result->getString(
                            "class_name"
                        ).c_str()
                    )
                );
            }

            this->cmbClass->SelectedIndex = 0;
        }


        void LoadExaminations()
        {
            try
            {
                int yearId =
                    GetSelectedId(
                        this->cmbAcademicYear
                    );

                int termId =
                    GetSelectedId(
                        this->cmbTerm
                    );

                int classId =
                    GetSelectedId(
                        this->cmbClass
                    );

                String^ status =
                    this->cmbStatus->SelectedItem == nullptr
                    ? L"All Statuses"
                    : this->cmbStatus->SelectedItem->ToString();

                auto con =
                    DbConnection::GetConnection();

                std::string sqlText =
                    "SELECT "
                    "e.examination_id, "
                    "e.examination_name, "
                    "COALESCE(e.examination_type, '') AS examination_type, "
                    "a.year_name, "
                    "t.term_name, "
                    "c.class_name, "
                    "e.start_date, "
                    "e.end_date, "
                    "e.status "
                    "FROM examinations e "
                    "INNER JOIN academic_years a "
                    "ON a.academic_year_id = e.academic_year_id "
                    "INNER JOIN terms t "
                    "ON t.term_id = e.term_id "
                    "INNER JOIN classes c "
                    "ON c.class_id = e.class_id "
                    "WHERE 1 = 1 ";

                if (yearId > 0)
                    sqlText += "AND e.academic_year_id = ? ";

                if (termId > 0)
                    sqlText += "AND e.term_id = ? ";

                if (classId > 0)
                    sqlText += "AND e.class_id = ? ";

                if (!status->Equals(
                        L"All Statuses",
                        StringComparison::OrdinalIgnoreCase))
                {
                    sqlText += "AND e.status = ? ";
                }

                sqlText +=
                    "ORDER BY "
                    "e.examination_id DESC";

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        sqlText
                    )
                );

                int index = 1;

                if (yearId > 0)
                    stmt->setInt(index++, yearId);

                if (termId > 0)
                    stmt->setInt(index++, termId);

                if (classId > 0)
                    stmt->setInt(index++, classId);

                if (!status->Equals(
                        L"All Statuses",
                        StringComparison::OrdinalIgnoreCase))
                {
                    stmt->setString(
                        index++,
                        msclr::interop::marshal_as<std::string>(
                            status
                        )
                    );
                }

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                this->examinationsGrid->Rows->Clear();

                int count = 0;

                while (result->next())
                {
                    this->examinationsGrid->Rows->Add(
                        result->getInt(
                            "examination_id"
                        ),
                        gcnew String(
                            result->getString(
                                "examination_name"
                            ).c_str()
                        ),
                        gcnew String(
                            result->getString(
                                "examination_type"
                            ).c_str()
                        ),
                        gcnew String(
                            result->getString(
                                "year_name"
                            ).c_str()
                        ),
                        gcnew String(
                            result->getString(
                                "term_name"
                            ).c_str()
                        ),
                        gcnew String(
                            result->getString(
                                "class_name"
                            ).c_str()
                        ),
                        result->isNull("start_date")
                        ? L""
                        : gcnew String(
                            result->getString(
                                "start_date"
                            ).c_str()
                        ),
                        result->isNull("end_date")
                        ? L""
                        : gcnew String(
                            result->getString(
                                "end_date"
                            ).c_str()
                        ),
                        gcnew String(
                            result->getString(
                                "status"
                            ).c_str()
                        )
                    );

                    count++;
                }

                this->lblExamCount->Text =
                    L"Examinations: " +
                    count.ToString();

                UpdateActionState();
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(
                        ex.what()
                    ),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }


        void UpdateActionState()
        {
            bool selected =
                this->examinationsGrid->SelectedRows->Count > 0;

            this->btnEdit->Enabled = selected;
            this->btnSubjects->Enabled = selected;
            this->btnMarks->Enabled = false;
            this->btnResults->Enabled = false;
        }


        int GetSelectedExaminationId()
        {
            if (
                this->examinationsGrid->SelectedRows->Count == 0
            )
            {
                return 0;
            }

            return Convert::ToInt32(
                this->examinationsGrid
                    ->SelectedRows[0]
                    ->Cells["ExaminationId"]
                    ->Value
            );
        }


        void LoadExaminationFormData(
            ComboBox^ yearCombo,
            ComboBox^ termCombo,
            ComboBox^ classCombo,
            int selectedYearId,
            int selectedTermId,
            int selectedClassId)
        {
            auto con = DbConnection::GetConnection();

            yearCombo->Items->Clear();
            yearCombo->Items->Add(
                L"Select Academic Year"
            );

            std::unique_ptr<sql::PreparedStatement> years(
                con->prepareStatement(
                    "SELECT academic_year_id, year_name "
                    "FROM academic_years "
                    "WHERE status = 'Active' "
                    "ORDER BY academic_year_id DESC"
                )
            );

            std::unique_ptr<sql::ResultSet> yearResult(
                years->executeQuery()
            );

            int yearIndex = 0;
            int currentIndex = 0;

            while (yearResult->next())
            {
                int id =
                    yearResult->getInt(
                        "academic_year_id"
                    );

                AddFilterItem(
                    yearCombo,
                    id,
                    gcnew String(
                        yearResult->getString(
                            "year_name"
                        ).c_str()
                    )
                );

                currentIndex++;

                if (id == selectedYearId)
                    yearIndex = currentIndex;
            }

            yearCombo->SelectedIndex =
                yearIndex;

            termCombo->Items->Clear();
            termCombo->Items->Add(
                L"Select Term"
            );

            std::unique_ptr<sql::PreparedStatement> termsStmt(
                con->prepareStatement(
                    "SELECT term_id, term_name "
                    "FROM terms "
                    "WHERE academic_year_id = ? "
                    "ORDER BY term_id ASC"
                )
            );

            termsStmt->setInt(
                1,
                selectedYearId
            );

            std::unique_ptr<sql::ResultSet> termResult(
                termsStmt->executeQuery()
            );

            int termIndex = 0;
            currentIndex = 0;

            while (termResult->next())
            {
                int id =
                    termResult->getInt(
                        "term_id"
                    );

                AddFilterItem(
                    termCombo,
                    id,
                    gcnew String(
                        termResult->getString(
                            "term_name"
                        ).c_str()
                    )
                );

                currentIndex++;

                if (id == selectedTermId)
                    termIndex = currentIndex;
            }

            termCombo->SelectedIndex =
                termIndex;

            classCombo->Items->Clear();
            classCombo->Items->Add(
                L"Select Class"
            );

            std::unique_ptr<sql::PreparedStatement> classesStmt(
                con->prepareStatement(
                    "SELECT class_id, class_name "
                    "FROM classes "
                    "WHERE status = 'Active' "
                    "ORDER BY class_id ASC"
                )
            );

            std::unique_ptr<sql::ResultSet> classResult(
                classesStmt->executeQuery()
            );

            int classIndex = 0;
            currentIndex = 0;

            while (classResult->next())
            {
                int id =
                    classResult->getInt(
                        "class_id"
                    );

                AddFilterItem(
                    classCombo,
                    id,
                    gcnew String(
                        classResult->getString(
                            "class_name"
                        ).c_str()
                    )
                );

                currentIndex++;

                if (id == selectedClassId)
                    classIndex = currentIndex;
            }

            classCombo->SelectedIndex =
                classIndex;
        }


        void OpenExaminationEditor(
            int examinationId)
        {
            bool editMode =
                examinationId > 0;

            Form^ editor =
                gcnew Form();

            editor->Text =
                editMode
                ? L"Edit Examination"
                : L"New Examination";

            editor->StartPosition =
                FormStartPosition::CenterParent;

            editor->FormBorderStyle =
                FormBorderStyle::FixedSingle;

            editor->MaximizeBox = false;
            editor->MinimizeBox = false;
            editor->ShowInTaskbar = false;

            editor->ClientSize =
                System::Drawing::Size(
                    760,
                    560
                );

            editor->MinimumSize =
                System::Drawing::Size(
                    760,
                    560
                );

            editor->BackColor =
                Color::FromArgb(
                    248,
                    250,
                    252
                );

            Panel^ header =
                gcnew Panel();

            header->Dock =
                DockStyle::Top;

            header->Height = 88;

            header->BackColor =
                Color::FromArgb(
                    30,
                    41,
                    59
                );

            header->Padding =
                Padding(
                    20,
                    10,
                    20,
                    10
                );

            Label^ title =
                gcnew Label();

            title->Dock =
                DockStyle::Top;

            title->Height = 34;

            title->Text =
                editMode
                ? L"Edit Examination"
                : L"New Examination";

            title->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    18.0F,
                    FontStyle::Bold
                );

            title->ForeColor =
                Color::White;

            Label^ subtitle =
                gcnew Label();

            subtitle->Dock =
                DockStyle::Fill;

            subtitle->Text =
                L"Define the examination, academic period and class.";

            subtitle->ForeColor =
                Color::Gainsboro;

            subtitle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.5F
                );

            header->Controls->Add(
                subtitle
            );

            header->Controls->Add(
                title
            );


            TableLayoutPanel^ formLayout =
                gcnew TableLayoutPanel();

            formLayout->Dock =
                DockStyle::Fill;

            formLayout->Padding =
                Padding(
                    24,
                    18,
                    24,
                    10
                );

            formLayout->ColumnCount = 4;
            formLayout->RowCount = 5;

            formLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Absolute,
                    130.0F
                )
            );

            formLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    50.0F
                )
            );

            formLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Absolute,
                    130.0F
                )
            );

            formLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    50.0F
                )
            );

            for (int i = 0; i < 4; i++)
            {
                formLayout->RowStyles->Add(
                    gcnew RowStyle(
                        SizeType::Absolute,
                        46.0F
                    )
                );
            }

            formLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    70.0F
                )
            );


            Label^ lblName =
                gcnew Label();

            lblName->Text =
                L"Examination Name";

            lblName->Dock =
                DockStyle::Fill;

            lblName->TextAlign =
                ContentAlignment::MiddleLeft;

            TextBox^ txtName =
                gcnew TextBox();

            txtName->Dock =
                DockStyle::Fill;

            txtName->Margin =
                Padding(
                    3,
                    5,
                    3,
                    5
                );


            Label^ lblType =
                gcnew Label();

            lblType->Text =
                L"Examination Type";

            lblType->Dock =
                DockStyle::Fill;

            lblType->TextAlign =
                ContentAlignment::MiddleLeft;

            ComboBox^ cmbType =
                gcnew ComboBox();

            cmbType->Dock =
                DockStyle::Fill;

            cmbType->DropDownStyle =
                ComboBoxStyle::DropDownList;

            array<String^>^ types = gcnew array<String^>
            {
                L"Beginning of Term",
                L"Mid-Term",
                L"End of Term",
                L"Mock Examination",
                L"Pre-UCE",
                L"Pre-UACE",
                L"Other"
            };

            for each (String^ type in types)
            {
                cmbType->Items->Add(type);
            }

            cmbType->SelectedIndex = 0;

            Label^ lblYear =
                gcnew Label();

            lblYear->Text =
                L"Academic Year";

            lblYear->Dock =
                DockStyle::Fill;

            lblYear->TextAlign =
                ContentAlignment::MiddleLeft;

            ComboBox^ cmbYear =
                gcnew ComboBox();

            cmbYear->Dock =
                DockStyle::Fill;

            cmbYear->DropDownStyle =
                ComboBoxStyle::DropDownList;

            ComboBox^ cmbTerm =
                gcnew ComboBox();

            cmbTerm->Dock =
                DockStyle::Fill;

            cmbTerm->DropDownStyle =
                ComboBoxStyle::DropDownList;

            cmbTerm->Items->Add(
                L"Select Term"
            );

            cmbTerm->SelectedIndex = 0;
            cmbTerm->Enabled = false;


            Label^ lblTerm =
                gcnew Label();

            lblTerm->Text =
                L"Term";

            lblTerm->Dock =
                DockStyle::Fill;

            lblTerm->TextAlign =
                ContentAlignment::MiddleLeft;


            Label^ lblClass =
                gcnew Label();

            lblClass->Text =
                L"Class";

            lblClass->Dock =
                DockStyle::Fill;

            lblClass->TextAlign =
                ContentAlignment::MiddleLeft;

            ComboBox^ cmbClass =
                gcnew ComboBox();

            cmbClass->Dock =
                DockStyle::Fill;

            cmbClass->DropDownStyle =
                ComboBoxStyle::DropDownList;


            Label^ lblStart =
                gcnew Label();

            lblStart->Text =
                L"Start Date";

            lblStart->Dock =
                DockStyle::Fill;

            lblStart->TextAlign =
                ContentAlignment::MiddleLeft;

            DateTimePicker^ dtpStart =
                gcnew DateTimePicker();

            dtpStart->Dock =
                DockStyle::Fill;

            dtpStart->Format =
                DateTimePickerFormat::Custom;

            dtpStart->CustomFormat =
                L"dd/MM/yyyy";

            dtpStart->ShowUpDown = false;


            Label^ lblEnd =
                gcnew Label();

            lblEnd->Text =
                L"End Date";

            lblEnd->Dock =
                DockStyle::Fill;

            lblEnd->TextAlign =
                ContentAlignment::MiddleLeft;

            DateTimePicker^ dtpEnd =
                gcnew DateTimePicker();

            dtpEnd->Dock =
                DockStyle::Fill;

            dtpEnd->Format =
                DateTimePickerFormat::Custom;

            dtpEnd->CustomFormat =
                L"dd/MM/yyyy";


            Label^ lblStatus =
                gcnew Label();

            lblStatus->Text =
                L"Status";

            lblStatus->Dock =
                DockStyle::Fill;

            lblStatus->TextAlign =
                ContentAlignment::MiddleLeft;

            ComboBox^ cmbStatus =
                gcnew ComboBox();

            cmbStatus->Dock =
                DockStyle::Fill;

            cmbStatus->DropDownStyle =
                ComboBoxStyle::DropDownList;

            cmbStatus->Items->Add(L"Draft");
            cmbStatus->Items->Add(L"Published");
            cmbStatus->Items->Add(L"Completed");
            cmbStatus->SelectedIndex = 0;


            formLayout->Controls->Add(lblName, 0, 0);
            formLayout->Controls->Add(txtName, 1, 0);
            formLayout->Controls->Add(lblType, 2, 0);
            formLayout->Controls->Add(cmbType, 3, 0);

            formLayout->Controls->Add(lblYear, 0, 1);
            formLayout->Controls->Add(cmbYear, 1, 1);
            formLayout->Controls->Add(lblTerm, 2, 1);
            formLayout->Controls->Add(cmbTerm, 3, 1);

            formLayout->Controls->Add(lblClass, 0, 2);
            formLayout->Controls->Add(cmbClass, 1, 2);
            formLayout->Controls->Add(lblStart, 2, 2);
            formLayout->Controls->Add(dtpStart, 3, 2);

            formLayout->Controls->Add(lblEnd, 0, 3);
            formLayout->Controls->Add(dtpEnd, 1, 3);
            formLayout->Controls->Add(lblStatus, 2, 3);
            formLayout->Controls->Add(cmbStatus, 3, 3);


            Button^ btnCancel =
                gcnew Button();

            btnCancel->Text = L"Cancel";
            btnCancel->Size =
                System::Drawing::Size(
                    110,
                    38
                );

            btnCancel->Anchor =
                AnchorStyles::Right |
                AnchorStyles::Bottom;


            Button^ btnSave =
                gcnew Button();

            btnSave->Text =
                editMode
                ? L"Save Changes"
                : L"Create Examination";

            btnSave->Size =
                System::Drawing::Size(
                    160,
                    38
                );

            btnSave->BackColor =
                Color::FromArgb(
                    38,
                    117,
                    92
                );

            btnSave->ForeColor =
                Color::White;

            btnSave->FlatStyle =
                FlatStyle::Flat;

            btnSave->FlatAppearance->BorderSize = 0;

            btnSave->Anchor =
                AnchorStyles::Right |
                AnchorStyles::Bottom;


            formLayout->Controls->Add(
                btnCancel,
                2,
                4
            );

            formLayout->Controls->Add(
                btnSave,
                3,
                4
            );


            LoadExaminationFormData(
                cmbYear,
                cmbTerm,
                cmbClass,
                0,
                0,
                0
            );


            // Load existing record if editing.
            if (editMode)
            {
                try
                {
                    auto con = DbConnection::GetConnection();

                    std::unique_ptr<sql::PreparedStatement> stmt(
                        con->prepareStatement(
                            "SELECT "
                            "examination_name, "
                            "examination_type, "
                            "academic_year_id, "
                            "term_id, "
                            "class_id, "
                            "start_date, "
                            "end_date, "
                            "status "
                            "FROM examinations "
                            "WHERE examination_id = ? "
                            "LIMIT 1"
                        )
                    );

                    stmt->setInt(
                        1,
                        examinationId
                    );

                    std::unique_ptr<sql::ResultSet> result(
                        stmt->executeQuery()
                    );

                    if (result->next())
                    {
                        txtName->Text =
                            gcnew String(
                                result->getString(
                                    "examination_name"
                                ).c_str()
                            );

                        String^ type =
                            result->isNull(
                                "examination_type"
                            )
                            ? L""
                            : gcnew String(
                                result->getString(
                                    "examination_type"
                                ).c_str()
                            );

                        int typeIndex =
                            cmbType->Items->IndexOf(
                                type
                            );

                        if (typeIndex >= 0)
                            cmbType->SelectedIndex = typeIndex;

                        int yearId =
                            result->getInt(
                                "academic_year_id"
                            );

                        int termId =
                            result->getInt(
                                "term_id"
                            );

                        int classId =
                            result->getInt(
                                "class_id"
                            );

                        LoadExaminationFormData(
                            cmbYear,
                            cmbTerm,
                            cmbClass,
                            yearId,
                            termId,
                            classId
                        );

                        if (!result->isNull("start_date"))
                            dtpStart->Value =
                                DateTime::Parse(
                                    gcnew String(
                                        result->getString(
                                            "start_date"
                                        ).c_str()
                                    )
                                );

                        if (!result->isNull("end_date"))
                            dtpEnd->Value =
                                DateTime::Parse(
                                    gcnew String(
                                        result->getString(
                                            "end_date"
                                        ).c_str()
                                    )
                                );

                        String^ dbStatus =
                            gcnew String(
                                result->getString(
                                    "status"
                                ).c_str()
                            );

                        int statusIndex =
                            cmbStatus->Items->IndexOf(
                                dbStatus
                            );

                        if (statusIndex >= 0)
                            cmbStatus->SelectedIndex =
                                statusIndex;
                    }
                }
                catch (sql::SQLException& ex)
                {
                    MessageBox::Show(
                        gcnew String(
                            ex.what()
                        ),
                        L"Database Error",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Error
                    );
                }
            }


            cmbYear->SelectedIndexChanged +=
                gcnew EventHandler(
                    [cmbYear, cmbTerm](Object^, EventArgs^)
                    {
                        int yearId = 0;

                        FilterItem^ item =
                            dynamic_cast<FilterItem^>(
                                cmbYear->SelectedItem
                            );

                        if (item != nullptr)
                            yearId = item->Id;

                        cmbTerm->Items->Clear();
                        cmbTerm->Items->Add(
                            L"Select Term"
                        );

                        if (yearId == 0)
                        {
                            cmbTerm->SelectedIndex = 0;
                            cmbTerm->Enabled = false;
                            return;
                        }

                        try
                        {
                            auto con =
                                DbConnection::GetConnection();

                            std::unique_ptr<sql::PreparedStatement> stmt(
                                con->prepareStatement(
                                    "SELECT term_id, term_name "
                                    "FROM terms "
                                    "WHERE academic_year_id = ? "
                                    "ORDER BY term_id ASC"
                                )
                            );

                            stmt->setInt(
                                1,
                                yearId
                            );

                            std::unique_ptr<sql::ResultSet> result(
                                stmt->executeQuery()
                            );

                            while (result->next())
                            {
                                FilterItem^ newItem =
                                    gcnew FilterItem(
                                        result->getInt(
                                            "term_id"
                                        ),
                                        gcnew String(
                                            result->getString(
                                                "term_name"
                                            ).c_str()
                                        )
                                    );

                                cmbTerm->Items->Add(
                                    newItem
                                );
                            }

                            cmbTerm->SelectedIndex = 0;
                            cmbTerm->Enabled = true;
                        }
                        catch (sql::SQLException& ex)
                        {
                            MessageBox::Show(
                                gcnew String(
                                    ex.what()
                                ),
                                L"Database Error",
                                MessageBoxButtons::OK,
                                MessageBoxIcon::Error
                            );
                        }
                    }
                );


            btnCancel->Click +=
                gcnew EventHandler(
                    [editor](Object^, EventArgs^)
                    {
                        editor->Close();
                    }
                );


            btnSave->Click +=
                gcnew EventHandler(
                    [this,
                     editor,
                     editMode,
                     examinationId,
                     txtName,
                     cmbType,
                     cmbYear,
                     cmbTerm,
                     cmbClass,
                     dtpStart,
                     dtpEnd,
                     cmbStatus](Object^, EventArgs^)
                    {
                        if (
                            String::IsNullOrWhiteSpace(
                                txtName->Text
                            )
                        )
                        {
                            MessageBox::Show(
                                L"Enter an examination name.",
                                L"Validation",
                                MessageBoxButtons::OK,
                                MessageBoxIcon::Warning
                            );

                            txtName->Focus();
                            return;
                        }

                        int yearId =
                            GetSelectedId(
                                cmbYear
                            );

                        int termId =
                            GetSelectedId(
                                cmbTerm
                            );

                        int classId =
                            GetSelectedId(
                                cmbClass
                            );

                        if (yearId == 0 ||
                            termId == 0 ||
                            classId == 0)
                        {
                            MessageBox::Show(
                                L"Select an academic year, term and class.",
                                L"Validation",
                                MessageBoxButtons::OK,
                                MessageBoxIcon::Warning
                            );

                            return;
                        }

                        if (dtpEnd->Value.Date < dtpStart->Value.Date)
                        {
                            MessageBox::Show(
                                L"The end date cannot be before the start date.",
                                L"Validation",
                                MessageBoxButtons::OK,
                                MessageBoxIcon::Warning
                            );

                            return;
                        }

                        try
                        {
                            auto con =
                                DbConnection::GetConnection();

                            if (editMode)
                            {
                                std::unique_ptr<sql::PreparedStatement> stmt(
                                    con->prepareStatement(
                                        "UPDATE examinations "
                                        "SET examination_name = ?, "
                                        "examination_type = ?, "
                                        "academic_year_id = ?, "
                                        "term_id = ?, "
                                        "class_id = ?, "
                                        "start_date = ?, "
                                        "end_date = ?, "
                                        "status = ? "
                                        "WHERE examination_id = ?"
                                    )
                                );

                                stmt->setString(
                                    1,
                                    msclr::interop::marshal_as<std::string>(
                                        txtName->Text->Trim()
                                    )
                                );

                                stmt->setString(
                                    2,
                                    msclr::interop::marshal_as<std::string>(
                                        cmbType->SelectedItem == nullptr
                                        ? L""
                                        : cmbType->SelectedItem->ToString()
                                    )
                                );

                                stmt->setInt(3, yearId);
                                stmt->setInt(4, termId);
                                stmt->setInt(5, classId);

                                stmt->setString(
                                    6,
                                    msclr::interop::marshal_as<std::string>(
                                        dtpStart->Value.ToString("yyyy-MM-dd")
                                    )
                                );

                                stmt->setString(
                                    7,
                                    msclr::interop::marshal_as<std::string>(
                                        dtpEnd->Value.ToString("yyyy-MM-dd")
                                    )
                                );

                                stmt->setString(
                                    8,
                                    msclr::interop::marshal_as<std::string>(
                                        cmbStatus->SelectedItem->ToString()
                                    )
                                );

                                stmt->setInt(
                                    9,
                                    examinationId
                                );

                                stmt->executeUpdate();

                                MessageBox::Show(
                                    L"Examination updated successfully.",
                                    L"Examinations",
                                    MessageBoxButtons::OK,
                                    MessageBoxIcon::Information
                                );
                            }
                            else
                            {
                                std::unique_ptr<sql::PreparedStatement> stmt(
                                    con->prepareStatement(
                                        "INSERT INTO examinations "
                                        "("
                                        "academic_year_id, "
                                        "term_id, "
                                        "class_id, "
                                        "examination_name, "
                                        "examination_type, "
                                        "start_date, "
                                        "end_date, "
                                        "status"
                                        ") "
                                        "VALUES (?, ?, ?, ?, ?, ?, ?, ?)"
                                    )
                                );

                                stmt->setInt(1, yearId);
                                stmt->setInt(2, termId);
                                stmt->setInt(3, classId);

                                stmt->setString(
                                    4,
                                    msclr::interop::marshal_as<std::string>(
                                        txtName->Text->Trim()
                                    )
                                );

                                stmt->setString(
                                    5,
                                    msclr::interop::marshal_as<std::string>(
                                        cmbType->SelectedItem == nullptr
                                        ? L""
                                        : cmbType->SelectedItem->ToString()
                                    )
                                );

                                stmt->setString(
                                    6,
                                    msclr::interop::marshal_as<std::string>(
                                        dtpStart->Value.ToString("yyyy-MM-dd")
                                    )
                                );

                                stmt->setString(
                                    7,
                                    msclr::interop::marshal_as<std::string>(
                                        dtpEnd->Value.ToString("yyyy-MM-dd")
                                    )
                                );

                                stmt->setString(
                                    8,
                                    msclr::interop::marshal_as<std::string>(
                                        cmbStatus->SelectedItem->ToString()
                                    )
                                );

                                stmt->executeUpdate();

                                MessageBox::Show(
                                    L"Examination created successfully.",
                                    L"Examinations",
                                    MessageBoxButtons::OK,
                                    MessageBoxIcon::Information
                                );
                            }

                            editor->DialogResult =
                                DialogResult::OK;

                            editor->Close();

                            this->LoadExaminations();
                        }
                        catch (sql::SQLException& ex)
                        {
                            MessageBox::Show(
                                gcnew String(
                                    ex.what()
                                ),
                                L"Database Error",
                                MessageBoxButtons::OK,
                                MessageBoxIcon::Error
                            );
                        }
                    }
                );


            editor->Controls->Add(
                formLayout
            );

            editor->Controls->Add(
                header
            );

            editor->ShowDialog(
                this
            );
        }


        void OpenSubjectAssignment(
            int examinationId)
        {
            Form^ dialog =
                gcnew Form();

            dialog->Text =
                L"Examination Subjects";

            dialog->StartPosition =
                FormStartPosition::CenterParent;

            dialog->FormBorderStyle =
                FormBorderStyle::Sizable;

            dialog->MaximizeBox = true;
            dialog->MinimizeBox = true;
            dialog->ShowInTaskbar = false;
            dialog->ClientSize =
                System::Drawing::Size(
                    950,
                    620
                );

            dialog->MinimumSize =
                System::Drawing::Size(
                    850,
                    560
                );

            dialog->BackColor =
                Color::WhiteSmoke;

            TableLayoutPanel^ layout =
                gcnew TableLayoutPanel();

            layout->Dock =
                DockStyle::Fill;

            layout->Padding =
                Padding(
                    20
                );

            layout->ColumnCount = 1;
            layout->RowCount = 4;

            layout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    70.0F
                )
            );

            layout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    90.0F
                )
            );

            layout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Percent,
                    100.0F
                )
            );

            layout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    60.0F
                )
            );

            Label^ title =
                gcnew Label();

            title->Dock =
                DockStyle::Fill;

            title->Text =
                L"Examination Subjects";

            title->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    18.0F,
                    FontStyle::Bold
                );

            title->ForeColor =
                Color::FromArgb(
                    30,
                    41,
                    59
                );

            title->TextAlign =
                ContentAlignment::MiddleLeft;

            layout->Controls->Add(
                title,
                0,
                0
            );


            TableLayoutPanel^ entry =
                gcnew TableLayoutPanel();

            entry->Dock =
                DockStyle::Fill;

            entry->ColumnCount = 6;
            entry->RowCount = 1;

            float[] widths = { 95, 250, 90, 120, 90, 120 };

            for (int i = 0; i < 6; i++)
            {
                entry->ColumnStyles->Add(
                    gcnew ColumnStyle(
                        SizeType::Absolute,
                        widths[i]
                    )
                );
            }

            Label^ lblSubject =
                gcnew Label();

            lblSubject->Text = L"Subject";
            lblSubject->Dock = DockStyle::Fill;
            lblSubject->TextAlign =
                ContentAlignment::MiddleLeft;

            ComboBox^ cmbSubject =
                gcnew ComboBox();

            cmbSubject->Dock = DockStyle::Fill;
            cmbSubject->DropDownStyle =
                ComboBoxStyle::DropDownList;

            Label^ lblMax =
                gcnew Label();

            lblMax->Text = L"Max Score";
            lblMax->Dock = DockStyle::Fill;
            lblMax->TextAlign =
                ContentAlignment::MiddleLeft;

            NumericUpDown^ numMax =
                gcnew NumericUpDown();

            numMax->Minimum = 1;
            numMax->Maximum = 1000;
            numMax->Value = 100;
            numMax->Dock = DockStyle::Fill;
            numMax->DecimalPlaces = 2;

            Label^ lblPass =
                gcnew Label();

            lblPass->Text = L"Pass Mark";
            lblPass->Dock = DockStyle::Fill;
            lblPass->TextAlign =
                ContentAlignment::MiddleLeft;

            NumericUpDown^ numPass =
                gcnew NumericUpDown();

            numPass->Minimum = 0;
            numPass->Maximum = 1000;
            numPass->Value = 40;
            numPass->Dock = DockStyle::Fill;
            numPass->DecimalPlaces = 2;

            entry->Controls->Add(
                lblSubject,
                0,
                0
            );

            entry->Controls->Add(
                cmbSubject,
                1,
                0
            );

            entry->Controls->Add(
                lblMax,
                2,
                0
            );

            entry->Controls->Add(
                numMax,
                3,
                0
            );

            entry->Controls->Add(
                lblPass,
                4,
                0
            );

            entry->Controls->Add(
                numPass,
                5,
                0
            );

            layout->Controls->Add(
                entry,
                0,
                1
            );


            DataGridView^ grid =
                gcnew DataGridView();

            grid->Dock =
                DockStyle::Fill;

            grid->AllowUserToAddRows = false;
            grid->AllowUserToDeleteRows = false;
            grid->ReadOnly = true;
            grid->MultiSelect = false;
            grid->SelectionMode =
                DataGridViewSelectionMode::FullRowSelect;
            grid->RowHeadersVisible = false;

            grid->AutoGenerateColumns = false;

            DataGridViewTextBoxColumn^ idColumn =
                gcnew DataGridViewTextBoxColumn();

            idColumn->Name = L"ExaminationSubjectId";
            idColumn->Visible = false;

            DataGridViewTextBoxColumn^ subjectColumn =
                gcnew DataGridViewTextBoxColumn();

            subjectColumn->Name = L"Subject";
            subjectColumn->HeaderText = L"Subject";
            subjectColumn->AutoSizeMode =
                DataGridViewAutoSizeColumnMode::Fill;

            DataGridViewTextBoxColumn^ maxColumn =
                gcnew DataGridViewTextBoxColumn();

            maxColumn->Name = L"MaxScore";
            maxColumn->HeaderText = L"Max Score";
            maxColumn->Width = 110;

            DataGridViewTextBoxColumn^ passColumn =
                gcnew DataGridViewTextBoxColumn();

            passColumn->Name = L"PassMark";
            passColumn->HeaderText = L"Pass Mark";
            passColumn->Width = 110;

            grid->Columns->Add(idColumn);
            grid->Columns->Add(subjectColumn);
            grid->Columns->Add(maxColumn);
            grid->Columns->Add(passColumn);


            // NOTE: Load the source subject list.
            try
            {
                auto con = DbConnection::GetConnection();

                int classId = 0;

                std::unique_ptr<sql::PreparedStatement> classStmt(
                    con->prepareStatement(
                        "SELECT class_id "
                        "FROM examinations "
                        "WHERE examination_id = ?"
                    )
                );

                classStmt->setInt(
                    1,
                    examinationId
                );

                std::unique_ptr<sql::ResultSet> classResult(
                    classStmt->executeQuery()
                );

                if (classResult->next())
                {
                    classId =
                        classResult->getInt(
                            "class_id"
                        );
                }

                std::unique_ptr<sql::PreparedStatement> subjectStmt(
                    con->prepareStatement(
                        "SELECT DISTINCT "
                        "s.subject_id, "
                        "s.subject_code, "
                        "s.subject_name "
                        "FROM class_subjects cs "
                        "INNER JOIN subjects s "
                        "ON s.subject_id = cs.subject_id "
                        "WHERE cs.class_id = ? "
                        "AND cs.status = 'Active' "
                        "AND s.status = 'Active' "
                        "ORDER BY s.subject_name ASC"
                    )
                );

                subjectStmt->setInt(
                    1,
                    classId
                );

                std::unique_ptr<sql::ResultSet> subjectResult(
                    subjectStmt->executeQuery()
                );

                cmbSubject->Items->Clear();

                while (subjectResult->next())
                {
                    cmbSubject->Items->Add(
                        gcnew FilterItem(
                            subjectResult->getInt(
                                "subject_id"
                            ),
                            gcnew String(
                                subjectResult->getString(
                                    "subject_name"
                                ).c_str()
                            ) +
                            L" (" +
                            gcnew String(
                                subjectResult->getString(
                                    "subject_code"
                                ).c_str()
                            ) +
                            L")"
                        )
                    );
                }

                if (cmbSubject->Items->Count > 0)
                    cmbSubject->SelectedIndex = 0;
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(
                        ex.what()
                    ),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }


            Button^ btnAdd =
                gcnew Button();

            btnAdd->Text =
                L"Add Subject";

            btnAdd->Size =
                System::Drawing::Size(
                    130,
                    38
                );


            Button^ btnRemove =
                gcnew Button();

            btnRemove->Text =
                L"Remove Subject";

            btnRemove->Size =
                System::Drawing::Size(
                    130,
                    38
                );


            Button^ btnClose =
                gcnew Button();

            btnClose->Text =
                L"Close";

            btnClose->Size =
                System::Drawing::Size(
                    110,
                    38
                );


            FlowLayoutPanel^ buttons =
                gcnew FlowLayoutPanel();

            buttons->Dock =
                DockStyle::Fill;

            buttons->FlowDirection =
                FlowDirection::RightToLeft;

            buttons->WrapContents = false;

            buttons->Padding =
                Padding(
                    0,
                    8,
                    0,
                    0
                );

            buttons->Controls->Add(btnClose);
            buttons->Controls->Add(btnRemove);
            buttons->Controls->Add(btnAdd);

            layout->Controls->Add(
                buttons,
                0,
                3
            );

            btnAdd->Click +=
                gcnew EventHandler(
                    [cmbSubject, numMax, numPass, examinationId, grid, this](Object^, EventArgs^)
                    {
                        FilterItem^ subject =
                            dynamic_cast<FilterItem^>(
                                cmbSubject->SelectedItem
                            );

                        if (subject == nullptr)
                        {
                            MessageBox::Show(
                                L"Select a subject first.",
                                L"Examination Subjects",
                                MessageBoxButtons::OK,
                                MessageBoxIcon::Warning
                            );
                            return;
                        }

                        if (numPass->Value > numMax->Value)
                        {
                            MessageBox::Show(
                                L"Pass mark cannot be greater than the maximum score.",
                                L"Validation",
                                MessageBoxButtons::OK,
                                MessageBoxIcon::Warning
                            );
                            return;
                        }

                        try
                        {
                            auto con =
                                DbConnection::GetConnection();

                            std::unique_ptr<sql::PreparedStatement> stmt(
                                con->prepareStatement(
                                    "INSERT INTO examination_subjects "
                                    "(examination_id, subject_id, max_score, pass_mark) "
                                    "VALUES (?, ?, ?, ?)"
                                )
                            );

                            stmt->setInt(
                                1,
                                examinationId
                            );

                            stmt->setInt(
                                2,
                                subject->Id
                            );

                            stmt->setDouble(
                                3,
                                Convert::ToDouble(
                                    numMax->Value
                                )
                            );

                            stmt->setDouble(
                                4,
                                Convert::ToDouble(
                                    numPass->Value
                                )
                            );

                            stmt->executeUpdate();

                            grid->Rows->Add(
                                subject->Id,
                                subject->Text,
                                numMax->Value.ToString("0.##"),
                                numPass->Value.ToString("0.##")
                            );

                            MessageBox::Show(
                                L"Subject added to examination.",
                                L"Examination Subjects",
                                MessageBoxButtons::OK,
                                MessageBoxIcon::Information
                            );

                            this->LoadExaminations();
                        }
                        catch (sql::SQLException& ex)
                        {
                            MessageBox::Show(
                                gcnew String(
                                    ex.what()
                                ),
                                L"Database Error",
                                MessageBoxButtons::OK,
                                MessageBoxIcon::Error
                            );
                        }
                    }
                );


            btnRemove->Click +=
                gcnew EventHandler(
                    [grid, examinationId, this](Object^, EventArgs^)
                    {
                        if (grid->SelectedRows->Count == 0)
                        {
                            MessageBox::Show(
                                L"Select a subject to remove.",
                                L"Examination Subjects",
                                MessageBoxButtons::OK,
                                MessageBoxIcon::Warning
                            );
                            return;
                        }

                        int examinationSubjectId =
                            Convert::ToInt32(
                                grid
                                    ->SelectedRows[0]
                                    ->Cells["ExaminationSubjectId"]
                                    ->Value
                            );

                        try
                        {
                            auto con =
                                DbConnection::GetConnection();

                            std::unique_ptr<sql::PreparedStatement> stmt(
                                con->prepareStatement(
                                    "DELETE FROM examination_subjects "
                                    "WHERE examination_subject_id = ?"
                                )
                            );

                            stmt->setInt(
                                1,
                                examinationSubjectId
                            );

                            int rowIndex =
                                grid->SelectedRows[0]->Index;

                            stmt->executeUpdate();

                            grid->Rows->RemoveAt(
                                rowIndex
                            );

                            MessageBox::Show(
                                L"Subject removed from examination.",
                                L"Examination Subjects",
                                MessageBoxButtons::OK,
                                MessageBoxIcon::Information
                            );
                        }
                        catch (sql::SQLException& ex)
                        {
                            MessageBox::Show(
                                gcnew String(
                                    ex.what()
                                ),
                                L"Database Error",
                                MessageBoxButtons::OK,
                                MessageBoxIcon::Error
                            );
                        }
                    }
                );


            btnClose->Click +=
                gcnew EventHandler(
                    [dialog](Object^, EventArgs^)
                    {
                        dialog->Close();
                    }
                );


            layout->Controls->Add(
                grid,
                0,
                2
            );

            dialog->Controls->Add(
                layout
            );

            dialog->ShowDialog(
                this
            );
        }


        System::Void btnNew_Click(
            Object^ sender,
            EventArgs^ e)
        {
            OpenExaminationEditor(
                0
            );
        }


        System::Void btnEdit_Click(
            Object^ sender,
            EventArgs^ e)
        {
            int id = GetSelectedExaminationId();

            if (id == 0)
            {
                MessageBox::Show(
                    L"Select an examination.",
                    L"Examinations",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                return;
            }

            OpenExaminationEditor(
                id
            );
        }


        System::Void btnSubjects_Click(
            Object^ sender,
            EventArgs^ e)
        {
            int id = GetSelectedExaminationId();

            if (id == 0)
                return;

            OpenSubjectAssignment(
                id
            );
        }


        System::Void btnMarks_Click(
            Object^ sender,
            EventArgs^ e)
        {
            // Marks entry will be enabled when the results workflow is added.
        }


        System::Void btnResults_Click(
            Object^ sender,
            EventArgs^ e)
        {
            // Results view will be enabled when the results workflow is added.
        }


        System::Void cmbAcademicYear_SelectedIndexChanged(
            Object^ sender,
            EventArgs^ e)
        {
            if (this->loadingFilters)
                return;

            try
            {
                this->loadingFilters = true;
                LoadTerms();
                this->loadingFilters = false;
                LoadExaminations();
            }
            catch (...)
            {
                this->loadingFilters = false;
                throw;
            }
        }


        System::Void FilterChanged(
            Object^ sender,
            EventArgs^ e)
        {
            if (!this->loadingFilters)
                LoadExaminations();
        }


        System::Void btnBack_Click(
            Object^ sender,
            EventArgs^ e)
        {
            this->Close();
        }


        void InitializeComponent()
        {
            this->SuspendLayout();

            this->Text =
                L"Examinations & Results";

            this->StartPosition =
                FormStartPosition::CenterScreen;

            this->WindowState =
                FormWindowState::Maximized;

            this->FormBorderStyle =
                FormBorderStyle::Sizable;

            this->MaximizeBox = true;
            this->MinimizeBox = true;

            this->ClientSize =
                System::Drawing::Size(
                    1180,
                    760
                );

            this->MinimumSize =
                System::Drawing::Size(
                    1000,
                    650
                );

            this->AutoScroll = true;
            this->BackColor =
                Color::FromArgb(
                    248,
                    250,
                    252
                );


            this->mainLayout =
                gcnew TableLayoutPanel();

            this->mainLayout->Dock =
                DockStyle::Fill;

            this->mainLayout->Padding =
                Padding(
                    20
                );

            this->mainLayout->ColumnCount = 1;
            this->mainLayout->RowCount = 5;

            this->mainLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    100.0F
                )
            );

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    84.0F
                )
            );

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    82.0F
                )
            );

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    30.0F
                )
            );

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Percent,
                    100.0F
                )
            );

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    64.0F
                )
            );


            // Header
            this->headerPanel =
                gcnew Panel();

            this->headerPanel->Dock =
                DockStyle::Fill;

            this->headerPanel->BackColor =
                Color::FromArgb(
                    30,
                    41,
                    59
                );

            this->headerPanel->Padding =
                Padding(
                    20,
                    9,
                    20,
                    9
                );


            this->lblTitle =
                gcnew Label();

            this->lblTitle->Dock =
                DockStyle::Top;

            this->lblTitle->Height =
                42;

            this->lblTitle->Text =
                L"Examinations & Results";

            this->lblTitle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    17.0F,
                    FontStyle::Bold
                );

            this->lblTitle->ForeColor =
                Color::White;

            this->lblTitle->TextAlign =
                ContentAlignment::MiddleLeft;


            this->lblSubtitle =
                gcnew Label();

            this->lblSubtitle->Dock =
                DockStyle::Fill;

            this->lblSubtitle->Text =
                L"Create examinations, configure their subjects and manage results.";

            this->lblSubtitle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.5F
                );

            this->lblSubtitle->ForeColor =
                Color::Gainsboro;

            this->lblSubtitle->TextAlign =
                ContentAlignment::MiddleLeft;


            this->headerPanel->Controls->Add(
                this->lblSubtitle
            );

            this->headerPanel->Controls->Add(
                this->lblTitle
            );


            // Filters
            this->filterPanel =
                gcnew Panel();

            this->filterPanel->Dock =
                DockStyle::Fill;

            this->filterPanel->BackColor =
                Color::White;

            this->filterPanel->Padding =
                Padding(
                    15,
                    10,
                    15,
                    8
                );


            TableLayoutPanel^ filterLayout =
                gcnew TableLayoutPanel();

            filterLayout->Dock =
                DockStyle::Fill;

            filterLayout->ColumnCount = 10;
            filterLayout->RowCount = 1;

            float filterWidths[10] =
                {
                    115, 170,
                    70, 150,
                    65, 150,
                    65, 135,
                    90, 100
                };

            for (int i = 0; i < 10; i++)
            {
                filterLayout->ColumnStyles->Add(
                    gcnew ColumnStyle(
                        SizeType::Absolute,
                        filterWidths[i]
                    )
                );
            }


            Label^ lblYear =
                gcnew Label();
            lblYear->Text = L"Academic Year";
            lblYear->Dock = DockStyle::Fill;
            lblYear->TextAlign =
                ContentAlignment::MiddleLeft;

            this->cmbAcademicYear =
                gcnew ComboBox();
            this->cmbAcademicYear->Dock =
                DockStyle::Fill;
            this->cmbAcademicYear->DropDownStyle =
                ComboBoxStyle::DropDownList;


            Label^ lblTerm =
                gcnew Label();
            lblTerm->Text = L"Term";
            lblTerm->Dock = DockStyle::Fill;
            lblTerm->TextAlign =
                ContentAlignment::MiddleLeft;

            this->cmbTerm =
                gcnew ComboBox();
            this->cmbTerm->Dock =
                DockStyle::Fill;
            this->cmbTerm->DropDownStyle =
                ComboBoxStyle::DropDownList;


            Label^ lblClass =
                gcnew Label();
            lblClass->Text = L"Class";
            lblClass->Dock = DockStyle::Fill;
            lblClass->TextAlign =
                ContentAlignment::MiddleLeft;

            this->cmbClass =
                gcnew ComboBox();
            this->cmbClass->Dock =
                DockStyle::Fill;
            this->cmbClass->DropDownStyle =
                ComboBoxStyle::DropDownList;


            Label^ lblStatus =
                gcnew Label();
            lblStatus->Text = L"Status";
            lblStatus->Dock = DockStyle::Fill;
            lblStatus->TextAlign =
                ContentAlignment::MiddleLeft;

            this->cmbStatus =
                gcnew ComboBox();
            this->cmbStatus->Dock =
                DockStyle::Fill;
            this->cmbStatus->DropDownStyle =
                ComboBoxStyle::DropDownList;

            this->cmbStatus->Items->Add(
                L"All Statuses"
            );
            this->cmbStatus->Items->Add(
                L"Draft"
            );
            this->cmbStatus->Items->Add(
                L"Published"
            );
            this->cmbStatus->Items->Add(
                L"Completed"
            );

            this->cmbStatus->SelectedIndex = 0;


            this->btnRefresh =
                gcnew Button();

            this->btnRefresh->Text =
                L"Refresh";

            this->btnRefresh->Size =
                System::Drawing::Size(
                    90,
                    34
                );

            this->btnRefresh->Anchor =
                AnchorStyles::Right;


            filterLayout->Controls->Add(
                lblYear,
                0,
                0
            );
            filterLayout->Controls->Add(
                this->cmbAcademicYear,
                1,
                0
            );
            filterLayout->Controls->Add(
                lblTerm,
                2,
                0
            );
            filterLayout->Controls->Add(
                this->cmbTerm,
                3,
                0
            );
            filterLayout->Controls->Add(
                lblClass,
                4,
                0
            );
            filterLayout->Controls->Add(
                this->cmbClass,
                5,
                0
            );
            filterLayout->Controls->Add(
                lblStatus,
                6,
                0
            );
            filterLayout->Controls->Add(
                this->cmbStatus,
                7,
                0
            );
            filterLayout->Controls->Add(
                this->btnRefresh,
                9,
                0
            );


            this->filterPanel->Controls->Add(
                filterLayout
            );


            // Count
            this->lblExamCount =
                gcnew Label();

            this->lblExamCount->Dock =
                DockStyle::Fill;

            this->lblExamCount->Text =
                L"Examinations: 0";

            this->lblExamCount->ForeColor =
                Color::DimGray;

            this->lblExamCount->TextAlign =
                ContentAlignment::MiddleLeft;


            // Grid
            this->examinationsGrid =
                gcnew DataGridView();

            this->examinationsGrid->Dock =
                DockStyle::Fill;

            this->examinationsGrid->ReadOnly =
                true;

            this->examinationsGrid->AllowUserToAddRows =
                false;

            this->examinationsGrid->AllowUserToDeleteRows =
                false;

            this->examinationsGrid->AllowUserToResizeRows =
                false;

            this->examinationsGrid->AutoGenerateColumns =
                false;

            this->examinationsGrid->SelectionMode =
                DataGridViewSelectionMode::FullRowSelect;

            this->examinationsGrid->MultiSelect =
                false;

            this->examinationsGrid->RowHeadersVisible =
                false;

            this->examinationsGrid->BackgroundColor =
                Color::White;

            this->examinationsGrid->BorderStyle =
                BorderStyle::None;

            this->examinationsGrid->ColumnHeadersHeight =
                42;

            this->examinationsGrid->EnableHeadersVisualStyles =
                false;

            this->examinationsGrid
                ->ColumnHeadersDefaultCellStyle
                ->BackColor =
                Color::FromArgb(
                    30,
                    41,
                    59
                );

            this->examinationsGrid
                ->ColumnHeadersDefaultCellStyle
                ->ForeColor =
                Color::White;

            this->examinationsGrid
                ->ColumnHeadersDefaultCellStyle
                ->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    9.5F,
                    FontStyle::Bold
                );

            this->examinationsGrid->RowTemplate->Height =
                34;


            DataGridViewTextBoxColumn^ id =
                gcnew DataGridViewTextBoxColumn();

            id->Name =
                L"ExaminationId";

            id->Visible = false;


            DataGridViewTextBoxColumn^ name =
                gcnew DataGridViewTextBoxColumn();

            name->Name =
                L"ExaminationName";

            name->HeaderText =
                L"Examination";

            name->Width =
                190;


            DataGridViewTextBoxColumn^ type =
                gcnew DataGridViewTextBoxColumn();

            type->Name =
                L"ExaminationType";

            type->HeaderText =
                L"Type";

            type->Width =
                145;


            DataGridViewTextBoxColumn^ year =
                gcnew DataGridViewTextBoxColumn();

            year->Name =
                L"AcademicYear";

            year->HeaderText =
                L"Academic Year";

            year->Width =
                120;


            DataGridViewTextBoxColumn^ term =
                gcnew DataGridViewTextBoxColumn();

            term->Name =
                L"Term";

            term->HeaderText =
                L"Term";

            term->Width =
                90;


            DataGridViewTextBoxColumn^ classColumn =
                gcnew DataGridViewTextBoxColumn();

            classColumn->Name =
                L"Class";

            classColumn->HeaderText =
                L"Class";

            classColumn->Width =
                100;


            DataGridViewTextBoxColumn^ start =
                gcnew DataGridViewTextBoxColumn();

            start->Name =
                L"StartDate";

            start->HeaderText =
                L"Start";

            start->Width =
                90;


            DataGridViewTextBoxColumn^ end =
                gcnew DataGridViewTextBoxColumn();

            end->Name =
                L"EndDate";

            end->HeaderText =
                L"End";

            end->Width =
                90;


            DataGridViewTextBoxColumn^ status =
                gcnew DataGridViewTextBoxColumn();

            status->Name =
                L"Status";

            status->HeaderText =
                L"Status";

            status->Width =
                100;


            this->examinationsGrid->Columns->Add(id);
            this->examinationsGrid->Columns->Add(name);
            this->examinationsGrid->Columns->Add(type);
            this->examinationsGrid->Columns->Add(year);
            this->examinationsGrid->Columns->Add(term);
            this->examinationsGrid->Columns->Add(classColumn);
            this->examinationsGrid->Columns->Add(start);
            this->examinationsGrid->Columns->Add(end);
            this->examinationsGrid->Columns->Add(status);


            // Actions
            this->actionPanel =
                gcnew FlowLayoutPanel();

            this->actionPanel->Dock =
                DockStyle::Fill;

            this->actionPanel->FlowDirection =
                FlowDirection::LeftToRight;

            this->actionPanel->WrapContents =
                false;

            this->actionPanel->Padding =
                Padding(
                    0,
                    8,
                    0,
                    0
                );


            this->btnNew =
                gcnew Button();

            this->btnNew->Text =
                L"New Examination";

            this->btnNew->Size =
                System::Drawing::Size(
                    140,
                    38
                );


            this->btnEdit =
                gcnew Button();

            this->btnEdit->Text =
                L"Edit";

            this->btnEdit->Enabled =
                false;

            this->btnEdit->Size =
                System::Drawing::Size(
                    95,
                    38
                );


            this->btnSubjects =
                gcnew Button();

            this->btnSubjects->Text =
                L"Manage Subjects";

            this->btnSubjects->Enabled =
                false;

            this->btnSubjects->Size =
                System::Drawing::Size(
                    140,
                    38
                );


            this->btnMarks =
                gcnew Button();

            this->btnMarks->Text =
                L"Enter Marks";

            this->btnMarks->Enabled =
                false;

            this->btnMarks->Size =
                System::Drawing::Size(
                    115,
                    38
                );


            this->btnResults =
                gcnew Button();

            this->btnResults->Text =
                L"View Results";

            this->btnResults->Enabled =
                false;

            this->btnResults->Size =
                System::Drawing::Size(
                    115,
                    38
                );


            this->btnBack =
                gcnew Button();

            this->btnBack->Text =
                L"Back to Dashboard";

            this->btnBack->Size =
                System::Drawing::Size(
                    150,
                    38
                );


            this->actionPanel->Controls->Add(
                this->btnNew
            );

            this->actionPanel->Controls->Add(
                this->btnEdit
            );

            this->actionPanel->Controls->Add(
                this->btnSubjects
            );

            this->actionPanel->Controls->Add(
                this->btnMarks
            );

            this->actionPanel->Controls->Add(
                this->btnResults
            );

            this->actionPanel->Controls->Add(
                this->btnBack
            );


            this->mainLayout->Controls->Add(
                this->headerPanel,
                0,
                0
            );

            this->mainLayout->Controls->Add(
                this->filterPanel,
                0,
                1
            );

            this->mainLayout->Controls->Add(
                this->lblExamCount,
                0,
                2
            );

            this->mainLayout->Controls->Add(
                this->examinationsGrid,
                0,
                3
            );

            this->mainLayout->Controls->Add(
                this->actionPanel,
                0,
                4
            );

            this->Controls->Add(
                this->mainLayout
            );


            this->cmbAcademicYear->SelectedIndexChanged +=
                gcnew EventHandler(
                    this,
                    &Examinations::cmbAcademicYear_SelectedIndexChanged
                );

            this->cmbTerm->SelectedIndexChanged +=
                gcnew EventHandler(
                    this,
                    &Examinations::FilterChanged
                );

            this->cmbClass->SelectedIndexChanged +=
                gcnew EventHandler(
                    this,
                    &Examinations::FilterChanged
                );

            this->cmbStatus->SelectedIndexChanged +=
                gcnew EventHandler(
                    this,
                    &Examinations::FilterChanged
                );

            this->btnRefresh->Click +=
                gcnew EventHandler(
                    [this](Object^, EventArgs^)
                    {
                        LoadExaminations();
                    }
                );

            this->examinationsGrid->SelectionChanged +=
                gcnew EventHandler(
                    [this](Object^, EventArgs^)
                    {
                        UpdateActionState();
                    }
                );

            this->btnNew->Click +=
                gcnew EventHandler(
                    this,
                    &Examinations::btnNew_Click
                );

            this->btnEdit->Click +=
                gcnew EventHandler(
                    this,
                    &Examinations::btnEdit_Click
                );

            this->btnSubjects->Click +=
                gcnew EventHandler(
                    this,
                    &Examinations::btnSubjects_Click
                );

            this->btnMarks->Click +=
                gcnew EventHandler(
                    this,
                    &Examinations::btnMarks_Click
                );

            this->btnResults->Click +=
                gcnew EventHandler(
                    this,
                    &Examinations::btnResults_Click
                );

            this->btnBack->Click +=
                gcnew EventHandler(
                    this,
                    &Examinations::btnBack_Click
                );

            this->ResumeLayout(false);
        }


    public:

        Examinations()
        {
            InitializeComponent();

            try
            {
                loadingFilters = true;

                LoadAcademicYears();
                LoadClasses();
                LoadTerms();

                loadingFilters = false;

                LoadExaminations();
            }
            catch (sql::SQLException& ex)
            {
                loadingFilters = false;

                MessageBox::Show(
                    gcnew String(
                        ex.what()
                    ),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }
    };
}

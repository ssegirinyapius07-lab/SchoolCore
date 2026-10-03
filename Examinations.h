#pragma once

#include "DbConnection.h"
#include "AuthSession.h"

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
    public ref class Examinations : public System::Windows::Forms::Form
    {
    public:
                Examinations(void)
                {
                    InitializeComponent();
        
                    if (System::ComponentModel::LicenseManager::UsageMode != System::ComponentModel::LicenseUsageMode::Designtime)
                    {
                        this->loadingFilters = true;
                        LoadAcademicYears();
                        LoadClasses();
                        LoadTerms();
                        this->loadingFilters = false;
                        LoadExaminations();
                    }
                }

        System::ComponentModel::Container^ components;


    protected:
        ~Examinations(void)
        {
            if (this->components)
            {
                delete this->components;
            }
        }

    private:

        ref class FilterItem
        {
        public:
            int Id;
            String^ Text;

            FilterItem(
                int id,
                String^ text)
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
        // MAIN FORM
        // =========================================================

        System::Windows::Forms::TableLayoutPanel^ mainLayout;
        System::Windows::Forms::Panel^ headerPanel;
        System::Windows::Forms::Label^ lblTitle;
        System::Windows::Forms::Label^ lblSubtitle;

        System::Windows::Forms::Panel^ filterPanel;
        System::Windows::Forms::ComboBox^ cmbAcademicYear;
        System::Windows::Forms::ComboBox^ cmbTerm;
        System::Windows::Forms::ComboBox^ cmbClass;
        System::Windows::Forms::ComboBox^ cmbStatus;
        System::Windows::Forms::Button^ btnRefresh;

        System::Windows::Forms::Label^ lblExamCount;
        System::Windows::Forms::DataGridView^ examinationsGrid;

        System::Windows::Forms::FlowLayoutPanel^ actionPanel;
        System::Windows::Forms::Button^ btnNew;
        System::Windows::Forms::Button^ btnEdit;
        System::Windows::Forms::Button^ btnSubjects;
        System::Windows::Forms::Button^ btnMarks;
        System::Windows::Forms::Button^ btnResults;
        System::Windows::Forms::Button^ btnBack;

        bool loadingFilters = false;


        // =========================================================
        // EXAMINATION EDITOR
        // =========================================================

        Form^ editorForm;
        System::Windows::Forms::TextBox^ editorName;
        System::Windows::Forms::ComboBox^ editorType;
        System::Windows::Forms::ComboBox^ editorYear;
        System::Windows::Forms::ComboBox^ editorTerm;
        System::Windows::Forms::ComboBox^ editorClass;
        System::Windows::Forms::DateTimePicker^ editorStart;
        System::Windows::Forms::DateTimePicker^ editorEnd;
        System::Windows::Forms::ComboBox^ editorStatus;
        bool editorEditMode = false;
        int editingExaminationId = 0;


        // =========================================================
        // SUBJECT ASSIGNMENT DIALOG
        // =========================================================

        Form^ subjectsDialog;
        System::Windows::Forms::ComboBox^ subjectCombo;
        System::Windows::Forms::NumericUpDown^ subjectMaxScore;
        System::Windows::Forms::NumericUpDown^ subjectPassMark;
        System::Windows::Forms::DataGridView^ assignedSubjectsGrid;
        int subjectAssignmentExaminationId = 0;


        // =========================================================
        // HELPERS
        // =========================================================

        int GetSelectedId(
            System::Windows::Forms::ComboBox^ combo)
        {
            if (
                combo == nullptr ||
                combo->SelectedItem == nullptr)
            {
                return 0;
            }

            FilterItem^ item =
                dynamic_cast<FilterItem^>(
                    combo->SelectedItem
                );

            return
                item == nullptr
                ? 0
                : item->Id;
        }


        void AddFilterItem(
            System::Windows::Forms::ComboBox^ combo,
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


        void AddFormLabel(
            System::Windows::Forms::TableLayoutPanel^ layout,
            String^ text,
            int column,
            int row)
        {
            System::Windows::Forms::Label^ label =
                gcnew System::Windows::Forms::Label();

            label->Text = text;
            label->Dock = DockStyle::Fill;
            label->TextAlign =
                ContentAlignment::MiddleLeft;
            label->Margin =
                System::Windows::Forms::Padding(
                    3,
                    0,
                    3,
                    0
                );
            label->AutoSize = false;

            layout->Controls->Add(
                label,
                column,
                row
            );
        }


        void ApplyEditorStyle(
            Control^ control)
        {
            control->Dock =
                DockStyle::Fill;

            control->Margin =
                System::Windows::Forms::Padding(
                    3,
                    4,
                    3,
                    4
                );
        }


        // =========================================================
        // FILTER LOADING
        // =========================================================

        void LoadAcademicYears()
        {
            try
            {
                auto con =
                    DbConnection::GetConnection();

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
            catch (sql::SQLException& ex)
            {
                ShowDatabaseError(ex);
            }
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
            catch (sql::SQLException& ex)
            {
                ShowDatabaseError(ex);
            }
        }


        void LoadClasses()
        {
            try
            {
                auto con =
                    DbConnection::GetConnection();

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
            catch (sql::SQLException& ex)
            {
                ShowDatabaseError(ex);
            }
        }


        // =========================================================
        // EXAMINATION LIST
        // =========================================================

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
                    sqlText +=
                        "AND e.academic_year_id = ? ";

                if (termId > 0)
                    sqlText +=
                        "AND e.term_id = ? ";

                if (classId > 0)
                    sqlText +=
                        "AND e.class_id = ? ";

                if (
                    !status->Equals(
                        L"All Statuses",
                        StringComparison::OrdinalIgnoreCase))
                {
                    sqlText +=
                        "AND e.status = ? ";
                }

                sqlText +=
                    "ORDER BY e.examination_id DESC";

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        sqlText
                    )
                );

                int index = 1;

                if (yearId > 0)
                    stmt->setInt(
                        index++,
                        yearId
                    );

                if (termId > 0)
                    stmt->setInt(
                        index++,
                        termId
                    );

                if (classId > 0)
                    stmt->setInt(
                        index++,
                        classId
                    );

                if (
                    !status->Equals(
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
                ShowDatabaseError(ex);
            }
        }


        void UpdateActionState()
        {
            bool selected =
                this->examinationsGrid->SelectedRows->Count > 0;

            this->btnEdit->Enabled =
                selected;

            this->btnSubjects->Enabled =
                selected;

            // These remain disabled until their real workflow is implemented.
            this->btnMarks->Enabled = false;
            this->btnResults->Enabled = false;
        }


        int GetSelectedExaminationId()
        {
            if (
                this->examinationsGrid->SelectedRows->Count == 0)
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


        void ShowDatabaseError(
            sql::SQLException& ex)
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


        // =========================================================
        // EDITOR DATA
        // =========================================================

        void LoadEditorYears(
            int selectedYearId)
        {
            this->editorYear->Items->Clear();

            this->editorYear->Items->Add(
                L"Select Academic Year"
            );

            try
            {
                auto con =
                    DbConnection::GetConnection();

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

                int index = 0;
                int selectedIndex = 0;

                while (result->next())
                {
                    index++;

                    int id =
                        result->getInt(
                            "academic_year_id"
                        );

                    AddFilterItem(
                        this->editorYear,
                        id,
                        gcnew String(
                            result->getString(
                                "year_name"
                            ).c_str()
                        )
                    );

                    if (id == selectedYearId)
                        selectedIndex = index;
                }

                this->editorYear->SelectedIndex =
                    selectedIndex;
            }
            catch (sql::SQLException& ex)
            {
                ShowDatabaseError(ex);
            }
        }


        void LoadEditorTerms(
            int selectedTermId)
        {
            int yearId =
                GetSelectedId(
                    this->editorYear
                );

            this->editorTerm->Items->Clear();

            this->editorTerm->Items->Add(
                L"Select Term"
            );

            this->editorTerm->Enabled =
                yearId > 0;

            if (yearId == 0)
            {
                this->editorTerm->SelectedIndex = 0;
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

                int index = 0;
                int selectedIndex = 0;

                while (result->next())
                {
                    index++;

                    int id =
                        result->getInt(
                            "term_id"
                        );

                    AddFilterItem(
                        this->editorTerm,
                        id,
                        gcnew String(
                            result->getString(
                                "term_name"
                            ).c_str()
                        )
                    );

                    if (id == selectedTermId)
                        selectedIndex = index;
                }

                this->editorTerm->SelectedIndex =
                    selectedIndex;
            }
            catch (sql::SQLException& ex)
            {
                ShowDatabaseError(ex);
            }
        }


        void LoadEditorClasses(
            int selectedClassId)
        {
            this->editorClass->Items->Clear();

            this->editorClass->Items->Add(
                L"Select Class"
            );

            try
            {
                auto con =
                    DbConnection::GetConnection();

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

                int index = 0;
                int selectedIndex = 0;

                while (result->next())
                {
                    index++;

                    int id =
                        result->getInt(
                            "class_id"
                        );

                    AddFilterItem(
                        this->editorClass,
                        id,
                        gcnew String(
                            result->getString(
                                "class_name"
                            ).c_str()
                        )
                    );

                    if (id == selectedClassId)
                        selectedIndex = index;
                }

                this->editorClass->SelectedIndex =
                    selectedIndex;
            }
            catch (sql::SQLException& ex)
            {
                ShowDatabaseError(ex);
            }
        }


        void LoadEditorForEdit()
        {
            if (!this->editorEditMode ||
                this->editingExaminationId == 0)
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
                    this->editingExaminationId
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                if (!result->next())
                    return;

                this->editorName->Text =
                    gcnew String(
                        result->getString(
                            "examination_name"
                        ).c_str()
                    );

                String^ type =
                    result->isNull(
                        "examination_type")
                    ? L""
                    : gcnew String(
                        result->getString(
                            "examination_type"
                        ).c_str()
                    );

                int typeIndex =
                    this->editorType->Items->IndexOf(
                        type
                    );

                if (typeIndex >= 0)
                    this->editorType->SelectedIndex =
                        typeIndex;

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

                this->editorYear->SelectedIndex = 0;
                LoadEditorYears(yearId);
                LoadEditorTerms(termId);
                LoadEditorClasses(classId);

                if (!result->isNull("start_date"))
                {
                    this->editorStart->Value =
                        DateTime::Parse(
                            gcnew String(
                                result->getString(
                                    "start_date"
                                ).c_str()
                            )
                        );
                }

                if (!result->isNull("end_date"))
                {
                    this->editorEnd->Value =
                        DateTime::Parse(
                            gcnew String(
                                result->getString(
                                    "end_date"
                                ).c_str()
                            )
                        );
                }

                String^ status =
                    gcnew String(
                        result->getString(
                            "status"
                        ).c_str()
                    );

                int statusIndex =
                    this->editorStatus->Items->IndexOf(
                        status
                    );

                if (statusIndex >= 0)
                    this->editorStatus->SelectedIndex =
                        statusIndex;
            }
            catch (sql::SQLException& ex)
            {
                ShowDatabaseError(ex);
            }
        }


        // =========================================================
        // EDITOR EVENTS
        // =========================================================

        System::Void EditorYearChanged(
            Object^ sender,
            EventArgs^ e)
        {
            if (this->editorYear == nullptr ||
                this->editorTerm == nullptr)
            {
                return;
            }

            LoadEditorTerms(0);
        }


        System::Void EditorCancelClicked(
            Object^ sender,
            EventArgs^ e)
        {
            if (this->editorForm != nullptr)
                this->editorForm->Close();
        }


        System::Void EditorSaveClicked(
            Object^ sender,
            EventArgs^ e)
        {
            if (
                String::IsNullOrWhiteSpace(
                    this->editorName->Text
                )
            )
            {
                MessageBox::Show(
                    L"Enter an examination name.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                this->editorName->Focus();
                return;
            }

            int yearId =
                GetSelectedId(
                    this->editorYear
                );

            int termId =
                GetSelectedId(
                    this->editorTerm
                );

            int classId =
                GetSelectedId(
                    this->editorClass
                );

            if (
                yearId == 0 ||
                termId == 0 ||
                classId == 0
            )
            {
                MessageBox::Show(
                    L"Select an academic year, term and class.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                return;
            }

            if (
                this->editorEnd->Value.Date <
                this->editorStart->Value.Date
            )
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

                std::string name =
                    msclr::interop::marshal_as<std::string>(
                        this->editorName->Text->Trim()
                    );

                std::string type =
                    msclr::interop::marshal_as<std::string>(
                        this->editorType->SelectedItem == nullptr
                        ? L""
                        : this->editorType->SelectedItem->ToString()
                    );

                std::string startDate =
                    msclr::interop::marshal_as<std::string>(
                        this->editorStart->Value.ToString(
                            L"yyyy-MM-dd"
                        )
                    );

                std::string endDate =
                    msclr::interop::marshal_as<std::string>(
                        this->editorEnd->Value.ToString(
                            L"yyyy-MM-dd"
                        )
                    );

                std::string status =
                    msclr::interop::marshal_as<std::string>(
                        this->editorStatus->SelectedItem == nullptr
                        ? L"Draft"
                        : this->editorStatus->SelectedItem->ToString()
                    );

                if (this->editorEditMode)
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

                    stmt->setString(1, name);
                    stmt->setString(2, type);
                    stmt->setInt(3, yearId);
                    stmt->setInt(4, termId);
                    stmt->setInt(5, classId);
                    stmt->setString(6, startDate);
                    stmt->setString(7, endDate);
                    stmt->setString(8, status);
                    stmt->setInt(
                        9,
                        this->editingExaminationId
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
                    stmt->setString(4, name);
                    stmt->setString(5, type);
                    stmt->setString(6, startDate);
                    stmt->setString(7, endDate);
                    stmt->setString(8, status);

                    stmt->executeUpdate();

                    MessageBox::Show(
                        L"Examination created successfully.",
                        L"Examinations",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Information
                    );
                }

                if (this->editorForm != nullptr)
                {
                    this->editorForm->DialogResult =
                        System::Windows::Forms::DialogResult::OK;

                    this->editorForm->Close();
                }

                LoadExaminations();
            }
            catch (sql::SQLException& ex)
            {
                ShowDatabaseError(ex);
            }
        }


        void OpenExaminationEditor(
            int examinationId)
        {
            this->editorEditMode =
                examinationId > 0;

            this->editingExaminationId =
                examinationId;

            this->editorForm =
                gcnew Form();

            this->editorForm->Text =
                this->editorEditMode
                ? L"Edit Examination"
                : L"New Examination";

            this->editorForm->StartPosition =
                FormStartPosition::CenterParent;

            this->editorForm->FormBorderStyle =
                System::Windows::Forms::FormBorderStyle::FixedSingle;

            this->editorForm->MaximizeBox = false;
            this->editorForm->MinimizeBox = false;
            this->editorForm->ShowInTaskbar = false;

            this->editorForm->ClientSize =
                System::Drawing::Size(
                    760,
                    560
                );

            this->editorForm->MinimumSize =
                System::Drawing::Size(
                    760,
                    560
                );

            System::Windows::Forms::Panel^ header =
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
                System::Windows::Forms::Padding(
                    20,
                    10,
                    20,
                    10
                );

            System::Windows::Forms::Label^ title =
                gcnew System::Windows::Forms::Label();

            title->Dock =
                DockStyle::Top;

            title->Height = 34;

            title->Text =
                this->editorEditMode
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

            System::Windows::Forms::Label^ subtitle =
                gcnew System::Windows::Forms::Label();

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

            header->Controls->Add(subtitle);
            header->Controls->Add(title);


            System::Windows::Forms::TableLayoutPanel^ formLayout =
                gcnew TableLayoutPanel();

            formLayout->Dock =
                DockStyle::Fill;

            formLayout->Padding =
                System::Windows::Forms::Padding(
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

            for (int i = 0; i < 5; i++)
            {
                formLayout->RowStyles->Add(
                    gcnew RowStyle(
                        SizeType::Absolute,
                        i == 4 ? 62.0F : 46.0F
                    )
                );
            }


            this->editorName =
                gcnew TextBox();

            this->editorType =
                gcnew ComboBox();

            this->editorYear =
                gcnew ComboBox();

            this->editorTerm =
                gcnew ComboBox();

            this->editorClass =
                gcnew ComboBox();

            this->editorStart =
                gcnew DateTimePicker();

            this->editorEnd =
                gcnew DateTimePicker();

            this->editorStatus =
                gcnew ComboBox();


            this->editorType->DropDownStyle =
                ComboBoxStyle::DropDownList;

            this->editorType->Items->Add(
                L"Beginning / Mid"
            );
            this->editorType->Items->Add(
                L"End of Term"
            );
            this->editorType->Items->Add(
                L"Mock Examination"
            );
            this->editorType->Items->Add(
                L"Pre-UCE"
            );
            this->editorType->Items->Add(
                L"Pre-UACE"
            );
            this->editorType->Items->Add(
                L"Other"
            );
            this->editorType->SelectedIndex = 0;


            this->editorYear->DropDownStyle =
                ComboBoxStyle::DropDownList;

            this->editorTerm->DropDownStyle =
                ComboBoxStyle::DropDownList;

            this->editorClass->DropDownStyle =
                ComboBoxStyle::DropDownList;


            this->editorStart->Format =
                DateTimePickerFormat::Custom;

            this->editorStart->CustomFormat =
                L"dd/MM/yyyy";

            this->editorStart->Value =
                DateTime::Today;


            this->editorEnd->Format =
                DateTimePickerFormat::Custom;

            this->editorEnd->CustomFormat =
                L"dd/MM/yyyy";

            this->editorEnd->Value =
                DateTime::Today;


            this->editorStatus->DropDownStyle =
                ComboBoxStyle::DropDownList;

            this->editorStatus->Items->Add(
                L"Draft"
            );
            this->editorStatus->Items->Add(
                L"Published"
            );
            this->editorStatus->Items->Add(
                L"Completed"
            );
            this->editorStatus->SelectedIndex = 0;


            ApplyEditorStyle(this->editorName);
            ApplyEditorStyle(this->editorType);
            ApplyEditorStyle(this->editorYear);
            ApplyEditorStyle(this->editorTerm);
            ApplyEditorStyle(this->editorClass);
            ApplyEditorStyle(this->editorStart);
            ApplyEditorStyle(this->editorEnd);
            ApplyEditorStyle(this->editorStatus);


            AddFormLabel(
                formLayout,
                L"Examination Name",
                0,
                0
            );

            formLayout->Controls->Add(
                this->editorName,
                1,
                0
            );

            AddFormLabel(
                formLayout,
                L"Examination Type",
                2,
                0
            );

            formLayout->Controls->Add(
                this->editorType,
                3,
                0
            );


            AddFormLabel(
                formLayout,
                L"Academic Year",
                0,
                1
            );

            formLayout->Controls->Add(
                this->editorYear,
                1,
                1
            );

            AddFormLabel(
                formLayout,
                L"Term",
                2,
                1
            );

            formLayout->Controls->Add(
                this->editorTerm,
                3,
                1
            );


            AddFormLabel(
                formLayout,
                L"Class",
                0,
                2
            );

            formLayout->Controls->Add(
                this->editorClass,
                1,
                2
            );

            AddFormLabel(
                formLayout,
                L"Start Date",
                2,
                2
            );

            formLayout->Controls->Add(
                this->editorStart,
                3,
                2
            );


            AddFormLabel(
                formLayout,
                L"End Date",
                0,
                3
            );

            formLayout->Controls->Add(
                this->editorEnd,
                1,
                3
            );

            AddFormLabel(
                formLayout,
                L"Status",
                2,
                3
            );

            formLayout->Controls->Add(
                this->editorStatus,
                3,
                3
            );


            System::Windows::Forms::Button^ cancel =
                gcnew Button();

            cancel->Text =
                L"Cancel";

            cancel->Size =
                System::Drawing::Size(
                    110,
                    38
                );

            System::Windows::Forms::Button^ save =
                gcnew Button();

            save->Text =
                this->editorEditMode
                ? L"Save Changes"
                : L"Create Examination";

            save->Size =
                System::Drawing::Size(
                    160,
                    38
                );

            save->BackColor =
                Color::FromArgb(
                    38,
                    117,
                    92
                );

            save->ForeColor =
                Color::White;

            save->FlatStyle =
                FlatStyle::Flat;

            save->FlatAppearance->BorderSize =
                0;


            formLayout->Controls->Add(
                cancel,
                2,
                4
            );

            formLayout->Controls->Add(
                save,
                3,
                4
            );


            this->editorYear->SelectedIndexChanged +=
                gcnew EventHandler(
                    this,
                    &Examinations::EditorYearChanged
                );

            cancel->Click +=
                gcnew EventHandler(
                    this,
                    &Examinations::EditorCancelClicked
                );

            save->Click +=
                gcnew EventHandler(
                    this,
                    &Examinations::EditorSaveClicked
                );


            this->LoadEditorYears(
                0
            );

            if (this->editorEditMode)
                this->LoadEditorForEdit();


            this->editorForm->Controls->Add(
                formLayout
            );

            this->editorForm->Controls->Add(
                header
            );

            this->editorForm->ShowDialog(
                this
            );
        }


        // =========================================================
        // SUBJECT ASSIGNMENT
        // =========================================================

        void LoadAssignedSubjects()
        {
            if (
                this->assignedSubjectsGrid == nullptr ||
                this->subjectAssignmentExaminationId == 0)
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
                        "es.examination_subject_id, "
                        "s.subject_name, "
                        "s.subject_code, "
                        "es.max_score, "
                        "es.pass_mark "
                        "FROM examination_subjects es "
                        "INNER JOIN subjects s "
                        "ON s.subject_id = es.subject_id "
                        "WHERE es.examination_id = ? "
                        "ORDER BY s.subject_name ASC"
                    )
                );

                stmt->setInt(
                    1,
                    this->subjectAssignmentExaminationId
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                this->assignedSubjectsGrid->Rows->Clear();

                while (result->next())
                {
                    this->assignedSubjectsGrid->Rows->Add(
                        result->getInt(
                            "examination_subject_id"
                        ),
                        gcnew String(
                            result->getString(
                                "subject_code"
                            ).c_str()
                        ),
                        gcnew String(
                            result->getString(
                                "subject_name"
                            ).c_str()
                        ),
                        result->getDouble(
                            "max_score"
                        ).ToString(
                            "0.##"
                        ),
                        result->isNull("pass_mark")
                        ? L""
                        : result->getDouble(
                            "pass_mark"
                        ).ToString(
                            "0.##"
                        )
                    );
                }
            }
            catch (sql::SQLException& ex)
            {
                ShowDatabaseError(ex);
            }
        }


        void LoadCombinationAwareSubjects()
        {
            this->subjectCombo->Items->Clear();

            int classId = 0;

            try
            {
                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> classStmt(
                    con->prepareStatement(
                        "SELECT "
                        "e.class_id "
                        "FROM examinations e "
                        "WHERE e.examination_id = ?"
                    )
                );

                classStmt->setInt(
                    1,
                    this->subjectAssignmentExaminationId
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

                std::unique_ptr<sql::ResultSet> result(
                    subjectStmt->executeQuery()
                );

                while (result->next())
                {
                    this->subjectCombo->Items->Add(
                        gcnew FilterItem(
                            result->getInt(
                                "subject_id"
                            ),
                            gcnew String(
                                result->getString(
                                    "subject_name"
                                ).c_str()
                            ) +
                            L" (" +
                            gcnew String(
                                result->getString(
                                    "subject_code"
                                ).c_str()
                            ) +
                            L")"
                        )
                    );
                }

                if (this->subjectCombo->Items->Count > 0)
                    this->subjectCombo->SelectedIndex = 0;
            }
            catch (sql::SQLException& ex)
            {
                ShowDatabaseError(ex);
            }
        }


        System::Void AddExamSubjectClicked(
            Object^ sender,
            EventArgs^ e)
        {
            FilterItem^ subject =
                dynamic_cast<FilterItem^>(
                    this->subjectCombo->SelectedItem
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

            if (
                this->subjectPassMark->Value >
                this->subjectMaxScore->Value
            )
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
                        "("
                        "examination_id, "
                        "subject_id, "
                        "max_score, "
                        "pass_mark"
                        ") "
                        "VALUES (?, ?, ?, ?)"
                    )
                );

                stmt->setInt(
                    1,
                    this->subjectAssignmentExaminationId
                );

                stmt->setInt(
                    2,
                    subject->Id
                );

                stmt->setDouble(
                    3,
                    Convert::ToDouble(
                        this->subjectMaxScore->Value
                    )
                );

                stmt->setDouble(
                    4,
                    Convert::ToDouble(
                        this->subjectPassMark->Value
                    )
                );

                stmt->executeUpdate();

                LoadAssignedSubjects();

                MessageBox::Show(
                    L"Subject added to examination.",
                    L"Examination Subjects",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information
                );
            }
            catch (sql::SQLException& ex)
            {
                ShowDatabaseError(ex);
            }
        }


        System::Void RemoveExamSubjectClicked(
            Object^ sender,
            EventArgs^ e)
        {
            if (
                this->assignedSubjectsGrid->SelectedRows->Count == 0)
            {
                MessageBox::Show(
                    L"Select a subject to remove.",
                    L"Examination Subjects",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                return;
            }

            int id =
                Convert::ToInt32(
                    this->assignedSubjectsGrid
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
                    id
                );

                stmt->executeUpdate();

                LoadAssignedSubjects();
            }
            catch (sql::SQLException& ex)
            {
                ShowDatabaseError(ex);
            }
        }


        System::Void CloseSubjectsDialogClicked(
            Object^ sender,
            EventArgs^ e)
        {
            if (this->subjectsDialog != nullptr)
                this->subjectsDialog->Close();
        }


        void OpenSubjectAssignment(
            int examinationId)
        {
            this->subjectAssignmentExaminationId =
                examinationId;

            this->subjectsDialog =
                gcnew Form();

            this->subjectsDialog->Text =
                L"Examination Subjects";

            this->subjectsDialog->StartPosition =
                FormStartPosition::CenterParent;

            this->subjectsDialog->FormBorderStyle =
                System::Windows::Forms::FormBorderStyle::Sizable;

            this->subjectsDialog->MaximizeBox = true;
            this->subjectsDialog->MinimizeBox = true;
            this->subjectsDialog->ShowInTaskbar = false;

            this->subjectsDialog->ClientSize =
                System::Drawing::Size(
                    1000,
                    650
                );

            this->subjectsDialog->MinimumSize =
                System::Drawing::Size(
                    900,
                    560
                );

            this->subjectsDialog->AutoScroll = true;

            System::Windows::Forms::TableLayoutPanel^ layout =
                gcnew TableLayoutPanel();

            layout->Dock =
                DockStyle::Fill;

            layout->Padding =
                System::Windows::Forms::Padding(
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
                    80.0F
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


            System::Windows::Forms::Label^ title =
                gcnew System::Windows::Forms::Label();

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


            System::Windows::Forms::TableLayoutPanel^ entry =
                gcnew TableLayoutPanel();

            entry->Dock =
                DockStyle::Fill;

            entry->ColumnCount = 6;
            entry->RowCount = 1;

            array<float>^ widths =
                gcnew array<float>
                {
                    95.0F,
                    250.0F,
                    90.0F,
                    120.0F,
                    90.0F,
                    120.0F
                };

            for (int i = 0; i < 6; i++)
            {
                entry->ColumnStyles->Add(
                    gcnew ColumnStyle(
                        SizeType::Absolute,
                        widths[i]
                    )
                );
            }


            System::Windows::Forms::Label^ subjectLabel =
                gcnew System::Windows::Forms::Label();

            subjectLabel->Text = L"Subject";
            subjectLabel->Dock = DockStyle::Fill;
            subjectLabel->TextAlign =
                ContentAlignment::MiddleLeft;

            this->subjectCombo =
                gcnew ComboBox();

            this->subjectCombo->Dock =
                DockStyle::Fill;

            this->subjectCombo->DropDownStyle =
                ComboBoxStyle::DropDownList;

            System::Windows::Forms::Label^ maxLabel =
                gcnew System::Windows::Forms::Label();

            maxLabel->Text =
                L"Max Score";

            maxLabel->Dock =
                DockStyle::Fill;

            maxLabel->TextAlign =
                ContentAlignment::MiddleLeft;

            this->subjectMaxScore =
                gcnew NumericUpDown();

            this->subjectMaxScore->Minimum =
                1;

            this->subjectMaxScore->Maximum =
                1000;

            this->subjectMaxScore->Value =
                100;

            this->subjectMaxScore->DecimalPlaces =
                2;

            this->subjectMaxScore->Dock =
                DockStyle::Fill;

            System::Windows::Forms::Label^ passLabel =
                gcnew System::Windows::Forms::Label();

            passLabel->Text =
                L"Pass Mark";

            passLabel->Dock =
                DockStyle::Fill;

            passLabel->TextAlign =
                ContentAlignment::MiddleLeft;

            this->subjectPassMark =
                gcnew NumericUpDown();

            this->subjectPassMark->Minimum =
                0;

            this->subjectPassMark->Maximum =
                1000;

            this->subjectPassMark->Value =
                40;

            this->subjectPassMark->DecimalPlaces =
                2;

            this->subjectPassMark->Dock =
                DockStyle::Fill;


            entry->Controls->Add(
                subjectLabel,
                0,
                0
            );

            entry->Controls->Add(
                this->subjectCombo,
                1,
                0
            );

            entry->Controls->Add(
                maxLabel,
                2,
                0
            );

            entry->Controls->Add(
                this->subjectMaxScore,
                3,
                0
            );

            entry->Controls->Add(
                passLabel,
                4,
                0
            );

            entry->Controls->Add(
                this->subjectPassMark,
                5,
                0
            );

            layout->Controls->Add(
                entry,
                0,
                1
            );


            this->assignedSubjectsGrid =
                gcnew DataGridView();

            this->assignedSubjectsGrid->Dock =
                DockStyle::Fill;

            this->assignedSubjectsGrid->AllowUserToAddRows = false;
            this->assignedSubjectsGrid->AllowUserToDeleteRows = false;
            this->assignedSubjectsGrid->ReadOnly = true;
            this->assignedSubjectsGrid->MultiSelect = false;

            this->assignedSubjectsGrid->SelectionMode =
                DataGridViewSelectionMode::FullRowSelect;

            this->assignedSubjectsGrid->RowHeadersVisible = false;
            this->assignedSubjectsGrid->AutoGenerateColumns = false;

            DataGridViewTextBoxColumn^ idColumn =
                gcnew DataGridViewTextBoxColumn();

            idColumn->Name =
                L"ExaminationSubjectId";

            idColumn->Visible = false;


            DataGridViewTextBoxColumn^ codeColumn =
                gcnew DataGridViewTextBoxColumn();

            codeColumn->Name =
                L"SubjectCode";

            codeColumn->HeaderText =
                L"Code";

            codeColumn->Width =
                130;


            DataGridViewTextBoxColumn^ nameColumn =
                gcnew DataGridViewTextBoxColumn();

            nameColumn->Name =
                L"SubjectName";

            nameColumn->HeaderText =
                L"Subject";

            nameColumn->AutoSizeMode =
                DataGridViewAutoSizeColumnMode::Fill;


            DataGridViewTextBoxColumn^ maxColumn =
                gcnew DataGridViewTextBoxColumn();

            maxColumn->Name =
                L"MaxScore";

            maxColumn->HeaderText =
                L"Max Score";

            maxColumn->Width =
                110;


            DataGridViewTextBoxColumn^ passColumn =
                gcnew DataGridViewTextBoxColumn();

            passColumn->Name =
                L"PassMark";

            passColumn->HeaderText =
                L"Pass Mark";

            passColumn->Width =
                110;


            this->assignedSubjectsGrid->Columns->Add(
                idColumn
            );

            this->assignedSubjectsGrid->Columns->Add(
                codeColumn
            );

            this->assignedSubjectsGrid->Columns->Add(
                nameColumn
            );

            this->assignedSubjectsGrid->Columns->Add(
                maxColumn
            );

            this->assignedSubjectsGrid->Columns->Add(
                passColumn
            );

            layout->Controls->Add(
                this->assignedSubjectsGrid,
                0,
                2
            );


            System::Windows::Forms::FlowLayoutPanel^ buttons =
                gcnew FlowLayoutPanel();

            buttons->Dock =
                DockStyle::Fill;

            buttons->FlowDirection =
                FlowDirection::RightToLeft;

            buttons->WrapContents = false;

            buttons->Padding =
                System::Windows::Forms::Padding(
                    0,
                    8,
                    0,
                    0
                );


            System::Windows::Forms::Button^ closeButton =
                gcnew Button();

            closeButton->Text =
                L"Close";

            closeButton->Size =
                System::Drawing::Size(
                    110,
                    38
                );


            System::Windows::Forms::Button^ removeButton =
                gcnew Button();

            removeButton->Text =
                L"Remove Subject";

            removeButton->Size =
                System::Drawing::Size(
                    135,
                    38
                );


            System::Windows::Forms::Button^ addButton =
                gcnew Button();

            addButton->Text =
                L"Add Subject";

            addButton->Size =
                System::Drawing::Size(
                    125,
                    38
                );


            buttons->Controls->Add(
                closeButton
            );

            buttons->Controls->Add(
                removeButton
            );

            buttons->Controls->Add(
                addButton
            );

            layout->Controls->Add(
                buttons,
                0,
                3
            );


            addButton->Click +=
                gcnew EventHandler(
                    this,
                    &Examinations::AddExamSubjectClicked
                );

            removeButton->Click +=
                gcnew EventHandler(
                    this,
                    &Examinations::RemoveExamSubjectClicked
                );

            closeButton->Click +=
                gcnew EventHandler(
                    this,
                    &Examinations::CloseSubjectsDialogClicked
                );


            this->LoadCombinationAwareSubjects();
            this->LoadAssignedSubjects();


            this->subjectsDialog->Controls->Add(
                layout
            );

            this->subjectsDialog->ShowDialog(
                this
            );
        }


        // =========================================================
        // MAIN EVENTS
        // =========================================================

        System::Void NewClicked(
            Object^ sender,
            EventArgs^ e)
        {
            OpenExaminationEditor(0);
        }


        System::Void EditClicked(
            Object^ sender,
            EventArgs^ e)
        {
            int id =
                GetSelectedExaminationId();

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

            OpenExaminationEditor(id);
        }


        System::Void SubjectsClicked(
            Object^ sender,
            EventArgs^ e)
        {
            int id =
                GetSelectedExaminationId();

            if (id != 0)
                OpenSubjectAssignment(id);
        }


        System::Void RefreshClicked(
            Object^ sender,
            EventArgs^ e)
        {
            LoadExaminations();
        }


        System::Void GridSelectionChanged(
            Object^ sender,
            EventArgs^ e)
        {
            UpdateActionState();
        }


        System::Void AcademicYearChanged(
            Object^ sender,
            EventArgs^ e)
        {
            if (this->loadingFilters)
                return;

            this->loadingFilters = true;

            LoadTerms();

            this->loadingFilters = false;

            LoadExaminations();
        }


        System::Void FilterChanged(
            Object^ sender,
            EventArgs^ e)
        {
            if (!this->loadingFilters)
                LoadExaminations();
        }


        System::Void BackClicked(
            Object^ sender,
            EventArgs^ e)
        {
            this->Close();
        }


        // =========================================================
        // INITIALIZE
        // =========================================================

        #pragma region Windows Form Designer generated code

void InitializeComponent(void)
        {
            this->components = gcnew System::ComponentModel::Container();
            this->SuspendLayout();


            // ---------------------------------------------------------
            // FORM
            // ---------------------------------------------------------

            this->Text =
                L"Examinations & Results";

            this->StartPosition =
                System::Windows::Forms::FormStartPosition::CenterScreen;

            this->WindowState =
                System::Windows::Forms::FormWindowState::Maximized;

            this->FormBorderStyle =
                System::Windows::Forms::FormBorderStyle::Sizable;

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
                System::Drawing::Color::FromArgb(
                    248,
                    250,
                    252
                );


            // ---------------------------------------------------------
            // MAIN LAYOUT
            // ---------------------------------------------------------

            this->mainLayout =
                gcnew System::Windows::Forms::TableLayoutPanel();

            this->mainLayout->Dock =
                System::Windows::Forms::DockStyle::Fill;

            this->mainLayout->Padding =
                System::Windows::Forms::Padding(
                    20
                );

            this->mainLayout->ColumnCount = 1;
            this->mainLayout->RowCount = 5;

            this->mainLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    System::Windows::Forms::SizeType::Percent,
                    100.0F
                )
            );

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    System::Windows::Forms::SizeType::Absolute,
                    84.0F
                )
            );

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    System::Windows::Forms::SizeType::Absolute,
                    82.0F
                )
            );

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    System::Windows::Forms::SizeType::Absolute,
                    30.0F
                )
            );

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    System::Windows::Forms::SizeType::Percent,
                    100.0F
                )
            );

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    System::Windows::Forms::SizeType::Absolute,
                    64.0F
                )
            );


            // ---------------------------------------------------------
            // HEADER
            // ---------------------------------------------------------

            this->headerPanel =
                gcnew System::Windows::Forms::Panel();

            this->headerPanel->Dock =
                System::Windows::Forms::DockStyle::Fill;

            this->headerPanel->BackColor =
                System::Drawing::Color::FromArgb(
                    30,
                    41,
                    59
                );

            this->headerPanel->Padding =
                System::Windows::Forms::Padding(
                    20,
                    9,
                    20,
                    9
                );


            this->lblTitle =
                gcnew System::Windows::Forms::Label();

            this->lblTitle->Dock =
                System::Windows::Forms::DockStyle::Top;

            this->lblTitle->Height =
                42;

            this->lblTitle->Text =
                L"Examinations & Results";

            this->lblTitle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    17.0F,
                    System::Drawing::FontStyle::Bold
                );

            this->lblTitle->ForeColor =
                System::Drawing::Color::White;

            this->lblTitle->TextAlign =
                System::Drawing::ContentAlignment::MiddleLeft;


            this->lblSubtitle =
                gcnew System::Windows::Forms::Label();

            this->lblSubtitle->Dock =
                System::Windows::Forms::DockStyle::Fill;

            this->lblSubtitle->Text =
                L"Create examinations, configure subjects and prepare results.";

            this->lblSubtitle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.5F
                );

            this->lblSubtitle->ForeColor =
                System::Drawing::Color::Gainsboro;

            this->lblSubtitle->TextAlign =
                System::Drawing::ContentAlignment::MiddleLeft;


            this->headerPanel->Controls->Add(
                this->lblSubtitle
            );

            this->headerPanel->Controls->Add(
                this->lblTitle
            );


            // ---------------------------------------------------------
            // FILTERS
            // ---------------------------------------------------------

            this->filterPanel =
                gcnew System::Windows::Forms::Panel();

            this->filterPanel->Dock =
                System::Windows::Forms::DockStyle::Fill;

            this->filterPanel->BackColor =
                System::Drawing::Color::White;

            this->filterPanel->Padding =
                System::Windows::Forms::Padding(
                    15,
                    10,
                    15,
                    8
                );


            System::Windows::Forms::TableLayoutPanel^ filterLayout =
                gcnew System::Windows::Forms::TableLayoutPanel();

            filterLayout->Dock =
                System::Windows::Forms::DockStyle::Fill;

            filterLayout->ColumnCount = 8;
            filterLayout->RowCount = 2;

            filterLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    System::Windows::Forms::SizeType::Absolute,
                    110.0F
                )
            );

            filterLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    System::Windows::Forms::SizeType::Percent,
                    25.0F
                )
            );

            filterLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    System::Windows::Forms::SizeType::Absolute,
                    55.0F
                )
            );

            filterLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    System::Windows::Forms::SizeType::Percent,
                    25.0F
                )
            );

            filterLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    System::Windows::Forms::SizeType::Absolute,
                    55.0F
                )
            );

            filterLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    System::Windows::Forms::SizeType::Percent,
                    25.0F
                )
            );

            filterLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    System::Windows::Forms::SizeType::Absolute,
                    55.0F
                )
            );

            filterLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    System::Windows::Forms::SizeType::Percent,
                    25.0F
                )
            );

            filterLayout->RowStyles->Add(
                gcnew RowStyle(
                    System::Windows::Forms::SizeType::Percent,
                    100.0F
                )
            );

            filterLayout->RowStyles->Add(
                gcnew RowStyle(
                    System::Windows::Forms::SizeType::Absolute,
                    1.0F
                )
            );


            System::Windows::Forms::Label^ yearLabel =
                gcnew System::Windows::Forms::Label();

            yearLabel->Text =
                L"Academic Year";

            yearLabel->Dock = System::Windows::Forms::DockStyle::Fill;
            yearLabel->TextAlign =
                System::Drawing::ContentAlignment::MiddleLeft;


            System::Windows::Forms::Label^ termLabel =
                gcnew System::Windows::Forms::Label();

            termLabel->Text =
                L"Term";

            termLabel->Dock = System::Windows::Forms::DockStyle::Fill;
            termLabel->TextAlign =
                System::Drawing::ContentAlignment::MiddleLeft;


            System::Windows::Forms::Label^ classLabel =
                gcnew System::Windows::Forms::Label();

            classLabel->Text =
                L"Class";

            classLabel->Dock = System::Windows::Forms::DockStyle::Fill;
            classLabel->TextAlign =
                System::Drawing::ContentAlignment::MiddleLeft;


            System::Windows::Forms::Label^ statusLabel =
                gcnew System::Windows::Forms::Label();

            statusLabel->Text =
                L"Status";

            statusLabel->Dock = System::Windows::Forms::DockStyle::Fill;
            statusLabel->TextAlign =
                System::Drawing::ContentAlignment::MiddleLeft;


            this->cmbAcademicYear =
                gcnew System::Windows::Forms::ComboBox();

            this->cmbTerm =
                gcnew System::Windows::Forms::ComboBox();

            this->cmbClass =
                gcnew System::Windows::Forms::ComboBox();

            this->cmbStatus =
                gcnew System::Windows::Forms::ComboBox();


            this->cmbAcademicYear->Dock =
                System::Windows::Forms::DockStyle::Fill;
            this->cmbAcademicYear->DropDownStyle =
                System::Windows::Forms::ComboBoxStyle::DropDownList;

            this->cmbTerm->Dock =
                System::Windows::Forms::DockStyle::Fill;
            this->cmbTerm->DropDownStyle =
                System::Windows::Forms::ComboBoxStyle::DropDownList;

            this->cmbClass->Dock =
                System::Windows::Forms::DockStyle::Fill;
            this->cmbClass->DropDownStyle =
                System::Windows::Forms::ComboBoxStyle::DropDownList;

            this->cmbStatus->Dock =
                System::Windows::Forms::DockStyle::Fill;
            this->cmbStatus->DropDownStyle =
                System::Windows::Forms::ComboBoxStyle::DropDownList;


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
                gcnew System::Windows::Forms::Button();

            this->btnRefresh->Text =
                L"Refresh";

            this->btnRefresh->Dock =
                System::Windows::Forms::DockStyle::Fill;

            this->btnRefresh->MinimumSize =
                System::Drawing::Size(
                    85,
                    34
                );


            filterLayout->Controls->Add(
                yearLabel,
                0,
                0
            );

            filterLayout->Controls->Add(
                this->cmbAcademicYear,
                1,
                0
            );

            filterLayout->Controls->Add(
                termLabel,
                2,
                0
            );

            filterLayout->Controls->Add(
                this->cmbTerm,
                3,
                0
            );

            filterLayout->Controls->Add(
                classLabel,
                4,
                0
            );

            filterLayout->Controls->Add(
                this->cmbClass,
                5,
                0
            );

            filterLayout->Controls->Add(
                statusLabel,
                6,
                0
            );

            filterLayout->Controls->Add(
                this->cmbStatus,
                7,
                0
            );

            this->filterPanel->Controls->Add(
                filterLayout
            );


            // ---------------------------------------------------------
            // COUNT
            // ---------------------------------------------------------

            this->lblExamCount =
                gcnew System::Windows::Forms::Label();

            this->lblExamCount->Dock =
                System::Windows::Forms::DockStyle::Fill;

            this->lblExamCount->Text =
                L"Examinations: 0";

            this->lblExamCount->ForeColor =
                System::Drawing::Color::DimGray;

            this->lblExamCount->TextAlign =
                System::Drawing::ContentAlignment::MiddleLeft;


            // ---------------------------------------------------------
            // GRID
            // ---------------------------------------------------------

            this->examinationsGrid =
                gcnew System::Windows::Forms::DataGridView();

            this->examinationsGrid->Dock =
                System::Windows::Forms::DockStyle::Fill;

            this->examinationsGrid->ReadOnly = true;

            this->examinationsGrid->AllowUserToAddRows = false;
            this->examinationsGrid->AllowUserToDeleteRows = false;
            this->examinationsGrid->AllowUserToResizeRows = false;

            this->examinationsGrid->AutoGenerateColumns =
                false;

            this->examinationsGrid->SelectionMode =
                System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;

            this->examinationsGrid->MultiSelect =
                false;

            this->examinationsGrid->RowHeadersVisible =
                false;

            this->examinationsGrid->BackgroundColor =
                System::Drawing::Color::White;

            this->examinationsGrid->BorderStyle =
                System::Windows::Forms::BorderStyle::None;

            this->examinationsGrid->ColumnHeadersHeight =
                42;

            this->examinationsGrid->RowTemplate->Height =
                34;

            this->examinationsGrid->EnableHeadersVisualStyles =
                false;

            this->examinationsGrid
                ->ColumnHeadersDefaultCellStyle
                ->BackColor =
                System::Drawing::Color::FromArgb(
                    30,
                    41,
                    59
                );

            this->examinationsGrid
                ->ColumnHeadersDefaultCellStyle
                ->ForeColor =
                System::Drawing::Color::White;

            this->examinationsGrid
                ->ColumnHeadersDefaultCellStyle
                ->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    9.5F,
                    System::Drawing::FontStyle::Bold
                );


            DataGridViewTextBoxColumn^ id =
                gcnew DataGridViewTextBoxColumn();

            id->Name =
                L"ExaminationId";

            id->Visible =
                false;


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


            // ---------------------------------------------------------
            // ACTIONS
            // ---------------------------------------------------------

            this->actionPanel =
                gcnew System::Windows::Forms::FlowLayoutPanel();

            this->actionPanel->Dock =
                System::Windows::Forms::DockStyle::Fill;

            this->actionPanel->FlowDirection =
                FlowDirection::LeftToRight;

            this->actionPanel->WrapContents =
                false;

            this->actionPanel->AutoScroll =
                true;

            this->actionPanel->Padding =
                System::Windows::Forms::Padding(
                    0,
                    8,
                    0,
                    0
                );


            this->btnNew =
                gcnew System::Windows::Forms::Button();

            this->btnNew->Text =
                L"New Examination";

            this->btnNew->Size =
                System::Drawing::Size(
                    140,
                    38
                );


            this->btnEdit =
                gcnew System::Windows::Forms::Button();

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
                gcnew System::Windows::Forms::Button();

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
                gcnew System::Windows::Forms::Button();

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
                gcnew System::Windows::Forms::Button();

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
                gcnew System::Windows::Forms::Button();

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


            // ---------------------------------------------------------
            // ADD TO MAIN LAYOUT
            // ---------------------------------------------------------

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


            // ---------------------------------------------------------
            // EVENTS
            // ---------------------------------------------------------

            this->cmbAcademicYear->SelectedIndexChanged +=
                gcnew EventHandler(
                    this,
                    &Examinations::AcademicYearChanged
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
                    this,
                    &Examinations::RefreshClicked
                );

            this->examinationsGrid->SelectionChanged +=
                gcnew EventHandler(
                    this,
                    &Examinations::GridSelectionChanged
                );

            this->btnNew->Click +=
                gcnew EventHandler(
                    this,
                    &Examinations::NewClicked
                );

            this->btnEdit->Click +=
                gcnew EventHandler(
                    this,
                    &Examinations::EditClicked
                );

            this->btnSubjects->Click +=
                gcnew EventHandler(
                    this,
                    &Examinations::SubjectsClicked
                );

            this->btnBack->Click +=
                gcnew EventHandler(
                    this,
                    &Examinations::BackClicked
                );


            this->ResumeLayout(false);
        }

#pragma endregion


    public:


    };
}

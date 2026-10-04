#pragma once

#include "DbConnection.h"
#include "AcademicContext.h"
#include "AcademicSecurity.h"
#include "ThemeManager.h"
#include "AuthSession.h"
#include "MarksEntryManagement.h"
#include "ResultsManagement.h"

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
        
                                        ThemeManager::ApplyToForm(this);
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
        System::Windows::Forms::ComboBox^ editorStream;
        System::Windows::Forms::DateTimePicker^ editorStart;
        System::Windows::Forms::DateTimePicker^ editorEnd;
        System::Windows::Forms::ComboBox^ editorStatus;
        System::Windows::Forms::ComboBox^ editorPolicy;
        bool editorEditMode = false;
        int editingExaminationId = 0;
        int editingApprovalId = 0;

        // Emergency override request dialog state
        Form^ overrideRequestForm;
        TextBox^ overrideReasonBox;
        int overrideRequestExaminationId = 0;
        int overrideRequestAcademicYearId = 0;
        int overrideRequestTermId = 0;
        int overrideCreatedApprovalId = 0;


        // =========================================================
        // SUBJECT ASSIGNMENT DIALOG
        // =========================================================

        Form^ subjectsDialog;
        System::Windows::Forms::ComboBox^ subjectCombo;
        System::Windows::Forms::ComboBox^ paperCombo;
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

        void LoadAcademicYears(){
            this->cmbAcademicYear->Items->Clear();
            this->cmbAcademicYear->Items->Add(
                L"All Academic Years"
            );

            try
            {
                AcademicYearInfo^ activeYear =
                    AcademicContext::GetActiveAcademicYear();

                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT academic_year_id, year_name, status "
                        "FROM academic_years "
                        "ORDER BY "
                        "CASE WHEN status = 'Active' THEN 0 ELSE 1 END, "
                        "academic_year_id DESC"
                    )
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                int selectedIndex = 0;
                int index = 0;

                while (result->next())
                {
                    index++;

                    int id =
                        result->getInt("academic_year_id");

                    AddFilterItem(
                        this->cmbAcademicYear,
                        id,
                        gcnew String(
                            result->getString("year_name").c_str()
                        )
                    );

                    if (id == activeYear->Id)
                        selectedIndex = index;
                }

                this->cmbAcademicYear->SelectedIndex =
                    selectedIndex;
            }
            catch (sql::SQLException& ex)
            {
                ShowDatabaseError(ex);
                this->cmbAcademicYear->SelectedIndex = 0;
            }
            catch (std::exception& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Academic Year",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );

                this->cmbAcademicYear->SelectedIndex = 0;
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

            std::unique_ptr<sql::Connection> con;

            try
            {
                con =
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
                    "COALESCE(st.stream_name, 'All Streams') AS stream_name, "
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
                    "LEFT JOIN streams st "
                    "ON st.stream_id = e.stream_id "
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
                        gcnew String(
                            result->getString(
                                "stream_name"
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

            this->btnMarks->Enabled =
                selected;

            this->btnResults->Enabled =
                selected;
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


        void LoadEditorStreams(
            int selectedStreamId)
        {
            this->editorStream->Items->Clear();

            this->editorStream->Items->Add(
                gcnew FilterItem(
                    0,
                    L"All Streams"
                )
            );

            int classId =
                GetSelectedId(
                    this->editorClass
                );

            if (classId <= 0)
            {
                this->editorStream->SelectedIndex = 0;
                this->editorStream->Enabled = false;
                return;
            }

            try
            {
                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT stream_id, stream_name "
                        "FROM streams "
                        "WHERE class_id = ? "
                        "AND status = 'Active' "
                        "ORDER BY stream_name ASC"
                    )
                );

                stmt->setInt(1, classId);

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                int selectedIndex = 0;
                int index = 0;

                while (result->next())
                {
                    index++;

                    int streamId =
                        result->getInt(
                            "stream_id"
                        );

                    this->editorStream->Items->Add(
                        gcnew FilterItem(
                            streamId,
                            gcnew String(
                                result->getString(
                                    "stream_name"
                                ).c_str()
                            )
                        )
                    );

                    if (streamId == selectedStreamId)
                        selectedIndex = index;
                }

                this->editorStream->SelectedIndex =
                    selectedIndex;

                this->editorStream->Enabled =
                    this->editorStream->Items->Count > 0;
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
                        "stream_id, "
                        "grading_policy_id, "
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

                int streamId =
                    result->isNull("stream_id")
                    ? 0
                    : result->getInt("stream_id");

                int policyId =
                    result->isNull("grading_policy_id")
                    ? 0
                    : result->getInt("grading_policy_id");

                this->editorYear->SelectedIndex = 0;
                LoadEditorYears(yearId);
                LoadEditorTerms(termId);
                LoadEditorClasses(classId);
                LoadEditorStreams(streamId);
                LoadEditorPolicies(policyId);

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
        void LoadEditorPolicies(
            int selectedPolicyId)
        {
            int classId =
                GetSelectedId(
                    this->editorClass
                );

            this->editorPolicy->Items->Clear();

            this->editorPolicy->Items->Add(
                gcnew FilterItem(
                    0,
                    L"Automatic - Current Curriculum"
                )
            );

            try
            {
                if (classId > 0)
                {
                    auto con =
                        DbConnection::GetConnection();

                    std::unique_ptr<sql::PreparedStatement> stmt(
                        con->prepareStatement(
                            "SELECT "
                            "gp.policy_id, "
                            "gp.policy_name "
                            "FROM grading_policies gp "
                            "INNER JOIN classes c "
                            "ON c.academic_level_id = gp.academic_level_id "
                            "WHERE c.class_id = ? "
                            "AND gp.status = 'Active' "
                            "ORDER BY gp.policy_type ASC, gp.policy_name ASC"
                        )
                    );

                    stmt->setInt(
                        1,
                        classId
                    );

                    std::unique_ptr<sql::ResultSet> result(
                        stmt->executeQuery()
                    );

                    while (result->next())
                    {
                        this->editorPolicy->Items->Add(
                            gcnew FilterItem(
                                result->getInt(
                                    "policy_id"
                                ),
                                gcnew String(
                                    result->getString(
                                        "policy_name"
                                    ).c_str()
                                )
                            )
                        );
                    }
                }
            }
            catch (sql::SQLException& ex)
            {
                ShowDatabaseError(ex);
            }

            int selectedIndex = 0;

            for (int i = 0;
                 i < this->editorPolicy->Items->Count;
                 i++)
            {
                FilterItem^ item =
                    dynamic_cast<FilterItem^>(
                        this->editorPolicy->Items[i]
                    );

                if (item != nullptr &&
                    item->Id == selectedPolicyId)
                {
                    selectedIndex = i;
                    break;
                }
            }

            this->editorPolicy->SelectedIndex =
                selectedIndex;
        }


        System::Void EditorClassChanged(
            Object^ sender,
            EventArgs^ e)
        {
            if (this->editorPolicy == nullptr ||
                this->editorStream == nullptr)
                return;

            LoadEditorStreams(0);
            LoadEditorPolicies(0);
        }




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

            int streamId =
                GetSelectedId(
                    this->editorStream
                );

            int policyId =
                GetSelectedId(
                    this->editorPolicy
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

            std::unique_ptr<sql::Connection> con;

            try
            {
                con =
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

                std::unique_ptr<sql::PreparedStatement> duplicateStmt(
                    con->prepareStatement(
                        "SELECT examination_id, stream_id "
                        "FROM examinations "
                        "WHERE academic_year_id = ? "
                        "AND term_id = ? "
                        "AND class_id = ? "
                        "AND TRIM(LOWER(COALESCE(examination_type, ''))) = "
                        "    TRIM(LOWER(?)) "
                        "AND examination_id <> ?"
                    )
                );

                duplicateStmt->setInt(
                    1,
                    yearId
                );
                duplicateStmt->setInt(
                    2,
                    termId
                );
                duplicateStmt->setInt(
                    3,
                    classId
                );
                duplicateStmt->setString(
                    4,
                    type
                );
                duplicateStmt->setInt(
                    5,
                    this->editorEditMode
                    ? this->editingExaminationId
                    : 0
                );

                std::unique_ptr<sql::ResultSet> duplicateResult(
                    duplicateStmt->executeQuery()
                );

                while (duplicateResult->next())
                {
                    int existingStreamId =
                        duplicateResult->isNull("stream_id")
                        ? 0
                        : duplicateResult->getInt("stream_id");

                    bool streamsOverlap =
                        existingStreamId == 0 ||
                        streamId == 0 ||
                        existingStreamId == streamId;

                    if (!streamsOverlap)
                        continue;

                    String^ existingScope =
                        existingStreamId == 0
                        ? L"All Streams"
                        : L"the selected stream";

                    String^ selectedScope =
                        streamId == 0
                        ? L"All Streams"
                        : L"the selected stream";

                    MessageBox::Show(
                        L"An examination with the same academic year, term, class and examination type already exists for " +
                        existingScope +
                        L". The selected scope (" +
                        selectedScope +
                        L") overlaps it, so it cannot be created.",
                        L"Overlapping Examination",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Warning
                    );

                    return;
                }

                if (this->editorEditMode &&
                    this->editingApprovalId > 0)
                {
                    if (!AcademicSecurity::HasApprovedOverride(
                            this->editingApprovalId,
                            yearId,
                            termId,
                            L"UPDATE",
                            L"examinations",
                            this->editingExaminationId.ToString()))
                    {
                        MessageBox::Show(
                            L"The approved emergency override is no longer valid for this exact examination. No changes were saved.",
                            L"Approval Required",
                            MessageBoxButtons::OK,
                            MessageBoxIcon::Warning
                        );
                        return;
                    }
                }

                con->setAutoCommit(false);

                int savedExaminationId =
                    this->editorEditMode
                    ? this->editingExaminationId
                    : 0;

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
                            "stream_id = ?, "
                            "grading_policy_id = ?, "
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

                    if (streamId == 0)
                        stmt->setNull(6, sql::DataType::INTEGER);
                    else
                        stmt->setInt(6, streamId);

                    if (policyId == 0)
                        stmt->setNull(7, sql::DataType::INTEGER);
                    else
                        stmt->setInt(7, policyId);

                    stmt->setString(8, startDate);
                    stmt->setString(9, endDate);
                    stmt->setString(10, status);
                    stmt->setInt(
                        11,
                        this->editingExaminationId
                    );

                    stmt->executeUpdate();
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
                            "stream_id, "
                            "examination_name, "
                            "examination_type, "
                            "grading_policy_id, "
                            "start_date, "
                            "end_date, "
                            "status"
                            ") "
                            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
                        )
                    );

                    stmt->setInt(1, yearId);
                    stmt->setInt(2, termId);
                    stmt->setInt(3, classId);

                    if (streamId == 0)
                        stmt->setNull(4, sql::DataType::INTEGER);
                    else
                        stmt->setInt(4, streamId);

                    stmt->setString(5, name);
                    stmt->setString(6, type);

                    if (policyId == 0)
                        stmt->setNull(7, sql::DataType::INTEGER);
                    else
                        stmt->setInt(7, policyId);

                    stmt->setString(8, startDate);
                    stmt->setString(9, endDate);
                    stmt->setString(10, status);

                    stmt->executeUpdate();

                    std::unique_ptr<sql::Statement> idStmt(
                        con->createStatement()
                    );

                    std::unique_ptr<sql::ResultSet> idResult(
                        idStmt->executeQuery(
                            "SELECT LAST_INSERT_ID() AS examination_id"
                        )
                    );

                    if (!idResult->next())
                    {
                        con->rollback();
                        con->setAutoCommit(true);
                        MessageBox::Show(
                            L"SchoolCore could not determine the new examination ID.",
                            L"Examinations",
                            MessageBoxButtons::OK,
                            MessageBoxIcon::Error
                        );
                        return;
                    }

                    savedExaminationId =
                        idResult->getInt(
                            "examination_id"
                        );
                }

                // Rebuild the database-enforced scope rows for this examination.
                std::unique_ptr<sql::PreparedStatement> deleteScopeStmt(
                    con->prepareStatement(
                        "DELETE FROM examination_stream_scopes "
                        "WHERE examination_id = ?"
                    )
                );

                deleteScopeStmt->setInt(
                    1,
                    savedExaminationId
                );

                deleteScopeStmt->executeUpdate();

                int scopeRowsInserted = 0;

                if (streamId == 0)
                {
                    std::unique_ptr<sql::PreparedStatement> scopeStmt(
                        con->prepareStatement(
                            "INSERT INTO examination_stream_scopes "
                            "("
                            "examination_id, "
                            "academic_year_id, "
                            "term_id, "
                            "class_id, "
                            "examination_name, "
                            "examination_type, "
                            "stream_id"
                            ") "
                            "SELECT ?, ?, ?, ?, ?, ?, s.stream_id "
                            "FROM streams s "
                            "WHERE s.class_id = ? "
                            "AND s.status = 'Active'"
                        )
                    );

                    scopeStmt->setInt(
                        1,
                        savedExaminationId
                    );
                    scopeStmt->setInt(
                        2,
                        yearId
                    );
                    scopeStmt->setInt(
                        3,
                        termId
                    );
                    scopeStmt->setInt(
                        4,
                        classId
                    );
                    scopeStmt->setString(
                        5,
                        name
                    );
                    scopeStmt->setString(
                        6,
                        type
                    );
                    scopeStmt->setInt(
                        7,
                        classId
                    );

                    scopeRowsInserted =
                        scopeStmt->executeUpdate();
                }
                else
                {
                    std::unique_ptr<sql::PreparedStatement> scopeStmt(
                        con->prepareStatement(
                            "INSERT INTO examination_stream_scopes "
                            "("
                            "examination_id, "
                            "academic_year_id, "
                            "term_id, "
                            "class_id, "
                            "examination_name, "
                            "examination_type, "
                            "stream_id"
                            ") "
                            "VALUES (?, ?, ?, ?, ?, ?, ?)"
                        )
                    );

                    scopeStmt->setInt(
                        1,
                        savedExaminationId
                    );
                    scopeStmt->setInt(
                        2,
                        yearId
                    );
                    scopeStmt->setInt(
                        3,
                        termId
                    );
                    scopeStmt->setInt(
                        4,
                        classId
                    );
                    scopeStmt->setString(
                        5,
                        name
                    );
                    scopeStmt->setString(
                        6,
                        type
                    );
                    scopeStmt->setInt(
                        7,
                        streamId
                    );

                    scopeRowsInserted =
                        scopeStmt->executeUpdate();
                }

                if (scopeRowsInserted <= 0)
                {
                    con->rollback();
                    con->setAutoCommit(true);

                    MessageBox::Show(
                        L"The selected class has no active stream available for this examination scope.",
                        L"Examinations",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Warning
                    );

                    return;
                }

                con->commit();
                con->setAutoCommit(true);
                this->editingApprovalId = 0;

                MessageBox::Show(
                    this->editorEditMode
                    ? L"Examination updated successfully."
                    : L"Examination created successfully.",
                    L"Examinations",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information
                );

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
                try
                {
                    if (con != nullptr)
                    {
                        con->rollback();
                        con->setAutoCommit(true);
                    }
                }
                catch (...)
                {
                }

                if (ex.getErrorCode() == 1062)
                {
                    MessageBox::Show(
                        L"The database rejected this examination because its stream scope overlaps an existing examination with the same academic year, term, class, name and type.",
                        L"Overlapping Examination",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Warning
                    );
                }
                else
                {
                    ShowDatabaseError(ex);
                }
            }
        }

        void SetNewExaminationDateDefaults()
        {
            int yearId =
                GetSelectedId(this->editorYear);

            if (yearId <= 0)
                return;

            DateTime fallbackStart =
                DateTime(
                    DateTime::Today.Year,
                    1,
                    1
                );

            DateTime fallbackEnd =
                DateTime(
                    DateTime::Today.Year,
                    12,
                    31
                );

            try
            {
                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT year_name, start_date, end_date "
                        "FROM academic_years "
                        "WHERE academic_year_id = ? "
                        "LIMIT 1"
                    )
                );

                stmt->setInt(1, yearId);

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                if (!result->next())
                    return;

                int academicYearNumber = 0;

                try
                {
                    academicYearNumber =
                        Convert::ToInt32(
                            gcnew String(
                                result->getString("year_name").c_str()
                            )
                        );
                }
                catch (...)
                {
                    academicYearNumber = DateTime::Today.Year;
                }

                if (academicYearNumber >= 1900 &&
                    academicYearNumber <= 2200)
                {
                    fallbackStart =
                        DateTime(
                            academicYearNumber,
                            1,
                            1
                        );

                    fallbackEnd =
                        DateTime(
                            academicYearNumber,
                            12,
                            31
                        );
                }

                DateTime startDate =
                    fallbackStart;

                DateTime endDate =
                    fallbackEnd;

                if (!result->isNull("start_date"))
                {
                    startDate =
                        DateTime::Parse(
                            gcnew String(
                                result->getString("start_date").c_str()
                            )
                        );
                }

                if (!result->isNull("end_date"))
                {
                    endDate =
                        DateTime::Parse(
                            gcnew String(
                                result->getString("end_date").c_str()
                            )
                        );
                }

                DateTime today =
                    DateTime::Today;

                DateTime defaultStart =
                    today >= startDate &&
                    today <= endDate
                    ? today
                    : startDate;

                DateTime defaultEnd =
                    today >= startDate &&
                    today <= endDate
                    ? today
                    : endDate;

                this->editorStart->Value =
                    defaultStart;

                this->editorEnd->Value =
                    defaultEnd;
            }
            catch (sql::SQLException& ex)
            {
                ShowDatabaseError(ex);
            }
            catch (System::Exception^)
            {
                this->editorStart->Value =
                    fallbackStart;

                this->editorEnd->Value =
                    fallbackEnd;
            }
        }



        void SubmitExaminationEditOverrideRequest(
            Object^ sender,
            EventArgs^ e)
        {
            if (this->overrideReasonBox == nullptr ||
                String::IsNullOrWhiteSpace(
                    this->overrideReasonBox->Text))
            {
                MessageBox::Show(
                    L"Enter a clear reason for the emergency correction.",
                    L"Reason Required",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                if (this->overrideReasonBox != nullptr)
                    this->overrideReasonBox->Focus();

                return;
            }

            try
            {
                this->overrideCreatedApprovalId =
                    AcademicSecurity::CreateInactiveYearEditRequest(
                        this->overrideRequestAcademicYearId,
                        this->overrideRequestTermId,
                        L"UPDATE",
                        L"examinations",
                        this->overrideRequestExaminationId.ToString(),
                        this->overrideReasonBox->Text->Trim()
                    );

                MessageBox::Show(
                    L"Emergency edit request #" +
                    this->overrideCreatedApprovalId.ToString() +
                    L" has been submitted for independent approval.",
                    L"Request Submitted",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information
                );

                this->overrideRequestForm->DialogResult =
                    System::Windows::Forms::DialogResult::OK;
            }
            catch (System::Exception^ ex)
            {
                MessageBox::Show(
                    ex->Message,
                    L"Unable to Submit Request",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }


        void CancelExaminationEditOverrideRequest(
            Object^ sender,
            EventArgs^ e)
        {
            if (this->overrideRequestForm != nullptr)
            {
                this->overrideRequestForm->DialogResult =
                    System::Windows::Forms::DialogResult::Cancel;
            }
        }


        int RequestExaminationEditOverride(
            int examinationId,
            int academicYearId,
            int termId)
        {
            this->overrideRequestExaminationId =
                examinationId;

            this->overrideRequestAcademicYearId =
                academicYearId;

            this->overrideRequestTermId =
                termId;

            this->overrideCreatedApprovalId =
                0;

            this->overrideRequestForm =
                gcnew Form();

            this->overrideRequestForm->Text =
                L"Request Emergency Override";

            this->overrideRequestForm->StartPosition =
                FormStartPosition::CenterParent;

            this->overrideRequestForm->FormBorderStyle =
                System::Windows::Forms::FormBorderStyle::FixedDialog;

            this->overrideRequestForm->MaximizeBox = false;
            this->overrideRequestForm->MinimizeBox = false;
            this->overrideRequestForm->ShowInTaskbar = false;

            this->overrideRequestForm->ClientSize =
                Drawing::Size(620, 360);

            Panel^ header =
                gcnew Panel();

            header->Dock =
                DockStyle::Top;

            header->Height = 82;

            header->BackColor =
                Color::FromArgb(30, 41, 59);

            Label^ title =
                gcnew Label();

            title->Text =
                L"Emergency Academic Edit Override";

            title->Dock =
                DockStyle::Top;

            title->Height = 34;

            title->Padding =
                System::Windows::Forms::Padding(16, 4, 8, 0);

            title->ForeColor =
                Color::White;

            title->Font =
                gcnew Drawing::Font(
                    L"Segoe UI Semibold",
                    15.0F,
                    FontStyle::Bold
                );

            Label^ subtitle =
                gcnew Label();

            subtitle->Text =
                L"An approval is required before this historical examination can be edited.";

            subtitle->Dock =
                DockStyle::Fill;

            subtitle->Padding =
                System::Windows::Forms::Padding(16, 0, 8, 6);

            subtitle->ForeColor =
                Color::FromArgb(226, 232, 240);

            header->Controls->Add(subtitle);
            header->Controls->Add(title);

            Label^ reasonLabel =
                gcnew Label();

            reasonLabel->Text =
                L"Reason for the emergency correction:";

            reasonLabel->Location =
                Drawing::Point(20, 102);

            reasonLabel->Size =
                Drawing::Size(560, 24);

            this->overrideReasonBox =
                gcnew TextBox();

            this->overrideReasonBox->Multiline = true;

            this->overrideReasonBox->ScrollBars =
                ScrollBars::Vertical;

            this->overrideReasonBox->Location =
                Drawing::Point(20, 130);

            this->overrideReasonBox->Size =
                Drawing::Size(580, 125);

            this->overrideReasonBox->Anchor =
                AnchorStyles::Top |
                AnchorStyles::Left |
                AnchorStyles::Right;

            Label^ scopeLabel =
                gcnew Label();

            scopeLabel->Text =
                L"Scope: UPDATE examinations / record ID " +
                examinationId.ToString();

            scopeLabel->Location =
                Drawing::Point(20, 265);

            scopeLabel->Size =
                Drawing::Size(580, 24);

            scopeLabel->ForeColor =
                Color::FromArgb(71, 85, 105);

            Button^ submitButton =
                gcnew Button();

            submitButton->Text =
                L"Submit Request";

            submitButton->Size =
                Drawing::Size(125, 36);

            submitButton->Location =
                Drawing::Point(340, 305);

            Button^ cancelButton =
                gcnew Button();

            cancelButton->Text =
                L"Cancel";

            cancelButton->Size =
                Drawing::Size(100, 36);

            cancelButton->Location =
                Drawing::Point(480, 305);

            submitButton->Click +=
                gcnew EventHandler(
                    this,
                    &Examinations::SubmitExaminationEditOverrideRequest
                );

            cancelButton->Click +=
                gcnew EventHandler(
                    this,
                    &Examinations::CancelExaminationEditOverrideRequest
                );

            this->overrideRequestForm->Controls->Add(cancelButton);
            this->overrideRequestForm->Controls->Add(submitButton);
            this->overrideRequestForm->Controls->Add(scopeLabel);
            this->overrideRequestForm->Controls->Add(this->overrideReasonBox);
            this->overrideRequestForm->Controls->Add(reasonLabel);
            this->overrideRequestForm->Controls->Add(header);

            this->overrideRequestForm->AcceptButton =
                submitButton;

            this->overrideRequestForm->CancelButton =
                cancelButton;

            this->overrideRequestForm->ShowDialog(this);

            int createdApprovalId =
                this->overrideCreatedApprovalId;

            delete this->overrideRequestForm;

            this->overrideRequestForm = nullptr;
            this->overrideReasonBox = nullptr;

            return createdApprovalId;
        }


        void OpenExaminationEditor(
            int examinationId)
        {
            if (examinationId > 0)
            {
                try
                {
                    auto securityCon =
                        DbConnection::GetConnection();

                    std::unique_ptr<sql::PreparedStatement> securityStmt(
                        securityCon->prepareStatement(
                            "SELECT academic_year_id, term_id "
                            "FROM examinations "
                            "WHERE examination_id = ? "
                            "LIMIT 1"
                        )
                    );

                    securityStmt->setInt(
                        1,
                        examinationId
                    );

                    std::unique_ptr<sql::ResultSet> securityResult(
                        securityStmt->executeQuery()
                    );

                    if (!securityResult->next())
                    {
                        MessageBox::Show(
                            L"The selected examination could not be found.",
                            L"Examinations",
                            MessageBoxButtons::OK,
                            MessageBoxIcon::Warning
                        );
                        return;
                    }

                    int existingYearId =
                        securityResult->getInt(
                            "academic_year_id"
                        );

                    int existingTermId =
                        securityResult->getInt(
                            "term_id"
                        );

                    if (!AcademicSecurity::IsCurrentActivePeriod(
                            existingYearId,
                            existingTermId))
                    {
                        this->editingApprovalId =
                            AcademicSecurity::GetApprovedOverrideId(
                                existingYearId,
                                existingTermId,
                                L"UPDATE",
                                L"examinations",
                                examinationId.ToString()
                            );

                        if (this->editingApprovalId == 0)
                        {
                            MessageBox::Show(
                                L"This examination belongs to an inactive academic year or inactive term. Normal editing is blocked.",
                                L"Protected Academic Record",
                                MessageBoxButtons::OK,
                                MessageBoxIcon::Warning
                            );

                            if (AcademicSecurity::HasInactiveYearOverridePermission())
                            {
                                if (MessageBox::Show(
                                        L"Would you like to submit an Emergency Override request for this examination?",
                                        L"Emergency Override",
                                        MessageBoxButtons::YesNo,
                                        MessageBoxIcon::Question) ==
                                    System::Windows::Forms::DialogResult::Yes)
                                {
                                    RequestExaminationEditOverride(
                                        examinationId,
                                        existingYearId,
                                        existingTermId
                                    );
                                }
                            }

                            return;
                        }

                        MessageBox::Show(
                            L"An approved emergency override was found for this examination. You may now make the authorized correction.",
                            L"Approved Emergency Override",
                            MessageBoxButtons::OK,
                            MessageBoxIcon::Information
                        );
                    }
                }
                catch (sql::SQLException& ex)
                {
                    ShowDatabaseError(ex);
                    return;
                }
            }
            this->editorEditMode =
                examinationId > 0;

            this->editingExaminationId =
                examinationId;
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
                    620
                );

            this->editorForm->MinimumSize =
                System::Drawing::Size(
                    760,
                    620
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
            formLayout->RowCount = 6;

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

            for (int i = 0; i < 6; i++)
            {
                formLayout->RowStyles->Add(
                    gcnew RowStyle(
                        SizeType::Absolute,
                        i == 5 ? 62.0F : 46.0F
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

            this->editorStream =
                gcnew ComboBox();

            this->editorStart =
                gcnew DateTimePicker();

            this->editorEnd =
                gcnew DateTimePicker();

            this->editorStatus =
                gcnew ComboBox();

            this->editorPolicy =
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

            this->editorStream->DropDownStyle =
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
            ApplyEditorStyle(this->editorStream);
            ApplyEditorStyle(this->editorStart);
            ApplyEditorStyle(this->editorEnd);
            ApplyEditorStyle(this->editorStatus);
            ApplyEditorStyle(this->editorPolicy);


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
                L"Stream",
                2,
                2
            );

            formLayout->Controls->Add(
                this->editorStream,
                3,
                2
            );


            AddFormLabel(
                formLayout,
                L"Start Date",
                0,
                3
            );

            formLayout->Controls->Add(
                this->editorStart,
                1,
                3
            );

            AddFormLabel(
                formLayout,
                L"End Date",
                2,
                3
            );

            formLayout->Controls->Add(
                this->editorEnd,
                3,
                3
            );


            AddFormLabel(
                formLayout,
                L"Status",
                0,
                4
            );

            formLayout->Controls->Add(
                this->editorStatus,
                1,
                4
            );

            AddFormLabel(
                formLayout,
                L"Grading Policy",
                2,
                4
            );

            formLayout->Controls->Add(
                this->editorPolicy,
                3,
                4
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
                5
            );

            formLayout->Controls->Add(
                save,
                3,
                5
            );


            this->editorYear->SelectedIndexChanged +=
                gcnew EventHandler(
                    this,
                    &Examinations::EditorYearChanged
                );

            this->editorClass->SelectedIndexChanged +=
                gcnew EventHandler(
                    this,
                    &Examinations::EditorClassChanged
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

            if (!this->editorEditMode)
            {
                this->SetNewExaminationDateDefaults();
            }

            this->LoadEditorClasses(
                0
            );

            this->LoadEditorStreams(0);
            this->LoadEditorPolicies(0);

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
                        "ep.examination_paper_id, "
                        "s.subject_code, "
                        "s.subject_name, "
                        "sp.paper_code, "
                        "sp.paper_name, "
                        "ep.max_score, "
                        "ep.pass_mark "
                        "FROM examination_papers ep "
                        "INNER JOIN examination_subjects es "
                        "ON es.examination_subject_id = ep.examination_subject_id "
                        "INNER JOIN subject_papers sp "
                        "ON sp.paper_id = ep.paper_id "
                        "INNER JOIN subjects s "
                        "ON s.subject_id = es.subject_id "
                        "WHERE es.examination_id = ? "
                        "AND ep.status = 'Active' "
                        "ORDER BY s.subject_name ASC, sp.paper_code ASC"
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
                    String^ paperName =
                        result->isNull("paper_name")
                        ? L""
                        : gcnew String(
                            result->getString(
                                "paper_name"
                            ).c_str()
                        );

                    this->assignedSubjectsGrid->Rows->Add(
                        result->getInt(
                            "examination_paper_id"
                        ),
                        result->isNull("subject_code")
                        ? L""
                        : gcnew String(
                            result->getString(
                                "subject_code"
                            ).c_str()
                        ),
                        gcnew String(
                            result->getString(
                                "subject_name"
                            ).c_str()
                        ),
                        gcnew String(
                            result->getString(
                                "paper_code"
                            ).c_str()
                        ),
                        paperName,
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
            this->paperCombo->Items->Clear();
            this->paperCombo->Enabled = false;

            try
            {
                auto con =
                    DbConnection::GetConnection();

                // Curriculum is resolved from the examination class's academic level.
                // The same academic year can contain both O-Level (S1-S4)
                // and A-Level (S5-S6), so the year-level curriculum flag must
                // not override the class level.
                std::unique_ptr<sql::PreparedStatement> subjectStmt(
                    con->prepareStatement(
                        "SELECT DISTINCT "
                        "s.subject_id, "
                        "s.subject_code, "
                        "s.subject_name, "
                        "cs.curriculum_subject_id "
                        "FROM examinations e "
                        "INNER JOIN academic_years ay "
                        "ON ay.academic_year_id = e.academic_year_id "
                        "INNER JOIN classes c "
                        "ON c.class_id = e.class_id "
                        "INNER JOIN curricula cur "
                        "ON cur.curriculum_id = ("
                        "SELECT c2.curriculum_id "
                        "FROM curricula c2 "
                        "WHERE c2.academic_level_id = c.academic_level_id "
                        "AND c2.status = 'Active' "
                        "ORDER BY c2.effective_from_year DESC, c2.curriculum_id DESC "
                        "LIMIT 1"
                        ") "
                        "INNER JOIN curriculum_subjects cs "
                        "ON cs.curriculum_id = cur.curriculum_id "
                        "AND cs.status = 'Active' "
                        "INNER JOIN subjects s "
                        "ON s.subject_id = cs.subject_id "
                        "AND s.status = 'Active' "
                        "WHERE e.examination_id = ? "
                        "ORDER BY s.subject_name ASC"
                    )
                );

                subjectStmt->setInt(
                    1,
                    this->subjectAssignmentExaminationId
                );

                std::unique_ptr<sql::ResultSet> result(
                    subjectStmt->executeQuery()
                );

                while (result->next())
                {
                    this->subjectCombo->Items->Add(
                        gcnew FilterItem(
                            result->getInt("subject_id"),
                            gcnew String(
                                result->getString("subject_name").c_str()
                            )
                        )
                    );
                }

                if (this->subjectCombo->Items->Count > 0)
                {
                    this->subjectCombo->SelectedIndex = 0;
                }
                else
                {
                    MessageBox::Show(
                        L"No active curriculum subjects were found for this examination's class and academic year. Check the curriculum and Curriculum Requirements setup.",
                        L"Examination Subjects",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Information
                    );
                }
            }
            catch (sql::SQLException& ex)
            {
                ShowDatabaseError(ex);
            }
        }


        void LoadPapersForSelectedSubject()
        {
            this->paperCombo->Items->Clear();
            this->paperCombo->Enabled = false;

            FilterItem^ subject =
                dynamic_cast<FilterItem^>(
                    this->subjectCombo->SelectedItem
                );

            if (subject == nullptr)
            {
                this->paperCombo->Items->Add(
                    L"Select paper"
                );

                this->paperCombo->SelectedIndex = 0;
                return;
            }

            try
            {
                auto con =
                    DbConnection::GetConnection();

                // Curriculum is resolved from the examination class's academic level.
                // Paper codes are shared across classes within that level
                // (for example S1-S4 use the UCE paper catalogue and S5-S6
                // use the UACE paper catalogue). Legacy unlinked paper rows
                // are still accepted when their subject and academic level match.
                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT DISTINCT "
                        "sp.paper_id, "
                        "sp.paper_code, "
                        "sp.paper_name "
                        "FROM examinations e "
                        "INNER JOIN academic_years ay "
                        "ON ay.academic_year_id = e.academic_year_id "
                        "INNER JOIN classes c "
                        "ON c.class_id = e.class_id "
                        "INNER JOIN subject_papers sp "
                        "ON sp.subject_id = ? "
                        "AND sp.academic_level_id = c.academic_level_id "
                        "AND sp.status = 'Active' "
                        "LEFT JOIN curriculum_subjects linked_cs "
                        "ON linked_cs.curriculum_subject_id = sp.curriculum_subject_id "
                        "AND linked_cs.status = 'Active' "
                        "LEFT JOIN curricula linked_cur "
                        "ON linked_cur.curriculum_id = linked_cs.curriculum_id "
                        "LEFT JOIN curricula active_cur "
                        "ON active_cur.curriculum_id = ("
                        "SELECT c2.curriculum_id "
                        "FROM curricula c2 "
                        "WHERE c2.academic_level_id = c.academic_level_id "
                        "AND c2.status = 'Active' "
                        "ORDER BY c2.effective_from_year DESC, c2.curriculum_id DESC "
                        "LIMIT 1"
                        ") "
                        "WHERE e.examination_id = ? "
                        "AND ("
                        "sp.curriculum_subject_id IS NULL "
                        "OR linked_cur.curriculum_id = active_cur.curriculum_id"
                        ") "
                        "ORDER BY sp.paper_code ASC"
                    )
                );

                stmt->setInt(
                    1,
                    subject->Id
                );

                stmt->setInt(
                    2,
                    this->subjectAssignmentExaminationId
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                while (result->next())
                {
                    String^ code =
                        gcnew String(
                            result->getString(
                                "paper_code"
                            ).c_str()
                        );

                    String^ name =
                        result->isNull("paper_name")
                        ? L""
                        : gcnew String(
                            result->getString(
                                "paper_name"
                            ).c_str()
                        );

                    String^ display =
                        String::IsNullOrWhiteSpace(name)
                        ? code
                        : code + L" - " + name;

                    this->paperCombo->Items->Add(
                        gcnew FilterItem(
                            result->getInt("paper_id"),
                            display
                        )
                    );
                }

                this->paperCombo->Enabled =
                    this->paperCombo->Items->Count > 0;

                if (this->paperCombo->Items->Count > 0)
                {
                    this->paperCombo->SelectedIndex = 0;
                }
                else
                {
                    this->paperCombo->Items->Add(
                        L"No papers configured"
                    );

                    this->paperCombo->SelectedIndex = 0;

                    MessageBox::Show(
                        L"No active UNEB papers are configured for this subject at this academic level. Open Subjects → UNEB Subject & Papers and verify that the official paper code is active.",
                        L"Examination Papers",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Information
                    );
                }
            }
            catch (sql::SQLException& ex)
            {
                ShowDatabaseError(ex);
            }
        }


        System::Void SubjectSelectionChanged(
            Object^ sender,
            EventArgs^ e)
        {
            this->LoadPapersForSelectedSubject();
        }


        System::Void AddExamSubjectClicked(
            Object^ sender,
            EventArgs^ e)
        {
            if (!EnsureExaminationWriteAccess(
                    this->subjectAssignmentExaminationId,
                    L"Adding examination subjects"))
            {
                return;
            }

            FilterItem^ subject =
                dynamic_cast<FilterItem^>(
                    this->subjectCombo->SelectedItem
                );

            FilterItem^ paper =
                dynamic_cast<FilterItem^>(
                    this->paperCombo->SelectedItem
                );

            if (subject == nullptr || paper == nullptr)
            {
                MessageBox::Show(
                    L"Select both a subject and its standard paper code.",
                    L"Examination Papers",
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

            std::unique_ptr<sql::Connection> con;

            try
            {
                con =
                    DbConnection::GetConnection();

                con->setAutoCommit(false);

                int examinationSubjectId = 0;

                std::unique_ptr<sql::PreparedStatement> findStmt(
                    con->prepareStatement(
                        "SELECT examination_subject_id "
                        "FROM examination_subjects "
                        "WHERE examination_id = ? "
                        "AND subject_id = ? "
                        "LIMIT 1"
                    )
                );

                findStmt->setInt(
                    1,
                    this->subjectAssignmentExaminationId
                );

                findStmt->setInt(
                    2,
                    subject->Id
                );

                std::unique_ptr<sql::ResultSet> findResult(
                    findStmt->executeQuery()
                );

                if (findResult->next())
                {
                    examinationSubjectId =
                        findResult->getInt(
                            "examination_subject_id"
                        );
                }
                else
                {
                    std::unique_ptr<sql::PreparedStatement> subjectStmt(
                        con->prepareStatement(
                            "INSERT INTO examination_subjects "
                            "(examination_id, subject_id, max_score, pass_mark) "
                            "VALUES (?, ?, ?, ?)"
                        )
                    );

                    subjectStmt->setInt(
                        1,
                        this->subjectAssignmentExaminationId
                    );

                    subjectStmt->setInt(
                        2,
                        subject->Id
                    );

                    subjectStmt->setDouble(
                        3,
                        Convert::ToDouble(
                            this->subjectMaxScore->Value
                        )
                    );

                    subjectStmt->setDouble(
                        4,
                        Convert::ToDouble(
                            this->subjectPassMark->Value
                        )
                    );

                    subjectStmt->executeUpdate();

                    std::unique_ptr<sql::Statement> idStmt(
                        con->createStatement()
                    );

                    std::unique_ptr<sql::ResultSet> idResult(
                        idStmt->executeQuery(
                            "SELECT LAST_INSERT_ID() AS examination_subject_id"
                        )
                    );

                    if (!idResult->next())
                    {
                        con->rollback();
                        con->setAutoCommit(true);
                        return;
                    }

                    examinationSubjectId =
                        idResult->getInt(
                            "examination_subject_id"
                        );
                }

                std::unique_ptr<sql::PreparedStatement> paperStmt(
                    con->prepareStatement(
                        "INSERT INTO examination_papers "
                        "(examination_subject_id, paper_id, max_score, pass_mark, status) "
                        "VALUES (?, ?, ?, ?, 'Active')"
                    )
                );

                paperStmt->setInt(
                    1,
                    examinationSubjectId
                );

                paperStmt->setInt(
                    2,
                    paper->Id
                );

                paperStmt->setDouble(
                    3,
                    Convert::ToDouble(
                        this->subjectMaxScore->Value
                    )
                );

                paperStmt->setDouble(
                    4,
                    Convert::ToDouble(
                        this->subjectPassMark->Value
                    )
                );

                paperStmt->executeUpdate();

                con->commit();
                con->setAutoCommit(true);

                LoadAssignedSubjects();

                MessageBox::Show(
                    L"Standard examination paper added successfully.",
                    L"Examination Papers",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information
                );
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

                ShowDatabaseError(ex);
            }
        }


        System::Void RemoveExamSubjectClicked(
            Object^ sender,
            EventArgs^ e)
        {
            if (!EnsureExaminationWriteAccess(
                    this->subjectAssignmentExaminationId,
                    L"Removing examination subjects"))
            {
                return;
            }

            if (
                this->assignedSubjectsGrid->SelectedRows->Count == 0)
            {
                MessageBox::Show(
                    L"Select an examination paper to remove.",
                    L"Examination Papers",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                return;
            }

            int id =
                Convert::ToInt32(
                    this->assignedSubjectsGrid
                        ->SelectedRows[0]
                        ->Cells["ExaminationPaperId"]
                        ->Value
                );

            try
            {
                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "DELETE FROM examination_papers "
                        "WHERE examination_paper_id = ?"
                    )
                );

                stmt->setInt(1, id);
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


        bool EnsureExaminationWriteAccess(
            int examinationId,
            String^ actionName)
        {
            if (examinationId <= 0)
            {
                MessageBox::Show(
                    L"A valid examination is required.",
                    L"Protected Academic Record",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                return false;
            }

            try
            {
                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT academic_year_id, term_id "
                        "FROM examinations "
                        "WHERE examination_id = ? "
                        "LIMIT 1"
                    )
                );

                stmt->setInt(1, examinationId);

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                if (!result->next())
                {
                    MessageBox::Show(
                        L"The selected examination could not be found.",
                        L"Examinations",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Warning
                    );
                    return false;
                }

                int yearId =
                    result->getInt("academic_year_id");

                int termId =
                    result->getInt("term_id");

                if (!AcademicSecurity::IsCurrentActivePeriod(
                        yearId,
                        termId))
                {
                    MessageBox::Show(
                        actionName +
                        L" is blocked because this examination belongs to an inactive academic year or inactive term. "
                        L"Historical records remain available for viewing, but changes require an approved academic-period override.",
                        L"Protected Academic Record",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Warning
                    );
                    return false;
                }

                return true;
            }
            catch (sql::SQLException& ex)
            {
                ShowDatabaseError(ex);
                return false;
            }
        }


        void OpenSubjectAssignment(
            int examinationId)
        {
            if (!EnsureExaminationWriteAccess(
                    examinationId,
                    L"Managing examination subjects"))
            {
                return;
            }

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

            entry->ColumnCount = 8;
            entry->RowCount = 1;

            array<float>^ widths =
                gcnew array<float>
                {
                    80.0F,
                    220.0F,
                    80.0F,
                    230.0F,
                    85.0F,
                    95.0F,
                    85.0F,
                    95.0F
                };

            for (int i = 0; i < 8; i++)
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

            System::Windows::Forms::Label^ paperLabel =
                gcnew System::Windows::Forms::Label();

            paperLabel->Text =
                L"Paper";

            paperLabel->Dock =
                DockStyle::Fill;

            paperLabel->TextAlign =
                ContentAlignment::MiddleLeft;

            this->paperCombo =
                gcnew ComboBox();

            this->paperCombo->Dock =
                DockStyle::Fill;

            this->paperCombo->DropDownStyle =
                ComboBoxStyle::DropDownList;

            this->paperCombo->Enabled = false;

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
                paperLabel,
                2,
                0
            );

            entry->Controls->Add(
                this->paperCombo,
                3,
                0
            );

            entry->Controls->Add(
                maxLabel,
                4,
                0
            );

            entry->Controls->Add(
                this->subjectMaxScore,
                5,
                0
            );

            entry->Controls->Add(
                passLabel,
                6,
                0
            );

            entry->Controls->Add(
                this->subjectPassMark,
                7,
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
                L"ExaminationPaperId";

            idColumn->Visible = false;


            DataGridViewTextBoxColumn^ codeColumn =
                gcnew DataGridViewTextBoxColumn();

            codeColumn->Name =
                L"SubjectCode";

            codeColumn->HeaderText =
                L"Subject Code";

            codeColumn->Width =
                120;


            DataGridViewTextBoxColumn^ nameColumn =
                gcnew DataGridViewTextBoxColumn();

            nameColumn->Name =
                L"SubjectName";

            nameColumn->HeaderText =
                L"Subject";

            nameColumn->Width =
                220;


            DataGridViewTextBoxColumn^ paperCodeColumn =
                gcnew DataGridViewTextBoxColumn();

            paperCodeColumn->Name =
                L"PaperCode";

            paperCodeColumn->HeaderText =
                L"Paper Code";

            paperCodeColumn->Width =
                120;


            DataGridViewTextBoxColumn^ paperNameColumn =
                gcnew DataGridViewTextBoxColumn();

            paperNameColumn->Name =
                L"PaperName";

            paperNameColumn->HeaderText =
                L"Paper";

            paperNameColumn->AutoSizeMode =
                DataGridViewAutoSizeColumnMode::Fill;


            DataGridViewTextBoxColumn^ maxColumn =
                gcnew DataGridViewTextBoxColumn();

            maxColumn->Name =
                L"MaxScore";

            maxColumn->HeaderText =
                L"Max Score";

            maxColumn->Width =
                95;


            DataGridViewTextBoxColumn^ passColumn =
                gcnew DataGridViewTextBoxColumn();

            passColumn->Name =
                L"PassMark";

            passColumn->HeaderText =
                L"Pass Mark";

            passColumn->Width =
                95;


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
                paperCodeColumn
            );

            this->assignedSubjectsGrid->Columns->Add(
                paperNameColumn
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
                L"Remove Paper";

            removeButton->Size =
                System::Drawing::Size(
                    135,
                    38
                );


            System::Windows::Forms::Button^ addButton =
                gcnew Button();

            addButton->Text =
                L"Add Paper";

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


            this->subjectCombo->SelectedIndexChanged +=
                gcnew EventHandler(
                    this,
                    &Examinations::SubjectSelectionChanged
                );

            this->LoadCombinationAwareSubjects();
            this->LoadPapersForSelectedSubject();
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


        System::Void MarksClicked(
            Object^ sender,
            EventArgs^ e)
        {
            int id =
                GetSelectedExaminationId();

            if (id == 0)
            {
                MessageBox::Show(
                    L"Select an examination first.",
                    L"Marks Entry",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                return;
            }

            if (!EnsureExaminationWriteAccess(
                    id,
                    L"Entering marks"))
            {
                return;
            }

            MarksEntryManagement^ form =
                gcnew MarksEntryManagement(id);

            form->ShowDialog(this);

            LoadExaminations();
        }


        System::Void ResultsClicked(
            Object^ sender,
            EventArgs^ e)
        {
            int id =
                GetSelectedExaminationId();

            if (id == 0)
            {
                MessageBox::Show(
                    L"Select an examination first.",
                    L"Results",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                return;
            }

            ResultsManagement^ form =
                gcnew ResultsManagement(id);

            form->ShowDialog(this);

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

            DataGridViewTextBoxColumn^ streamColumn =
                gcnew DataGridViewTextBoxColumn();

            streamColumn->Name =
                L"Stream";

            streamColumn->HeaderText =
                L"Stream";

            streamColumn->Width =
                105;

            this->examinationsGrid->Columns->Add(
                streamColumn
            );

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

            this->btnMarks->Click +=
                gcnew EventHandler(
                    this,
                    &Examinations::MarksClicked
                );

            this->btnResults->Click +=
                gcnew EventHandler(
                    this,
                    &Examinations::ResultsClicked
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

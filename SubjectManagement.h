#pragma once

#include "DbConnection.h"

#include <mariadb/conncpp.hpp>
#include <msclr/marshal_cppstd.h>

#include <memory>
#include <string>
#include <stdexcept>

using namespace System;
using namespace System::Drawing;
using namespace System::Windows::Forms;

namespace SchoolCore
{
    public ref class SubjectManagement : public Form
    {
    private:
        System::ComponentModel::Container^ components;
        // =========================================================
        // MAIN FORM
        // =========================================================

        TableLayoutPanel^ mainLayout;
        Panel^ headerPanel;
        System::Windows::Forms::Label^ lblTitle;
        System::Windows::Forms::Label^ lblSubtitle;

        Panel^ actionPanel;
        Button^ btnRegisterSubject;
        System::Windows::Forms::Label^ lblSearch;
        TextBox^ txtSearch;
        Button^ btnSearch;

        System::Windows::Forms::Label^ lblSubjectCount;
        DataGridView^ subjectsGrid;

        FlowLayoutPanel^ buttonPanel;
        Button^ btnViewProfile;
        Button^ btnEditSubject;
        Button^ btnToggleSubject;
        Button^ btnBack;

        // =========================================================
        // EDITOR DIALOG
        // =========================================================

        Form^ editorForm;
        TextBox^ txtSubjectCode;
        TextBox^ txtSubjectName;
        TextBox^ txtDescription;
        Button^ btnEditorSave;
        Button^ btnEditorCancel;

        bool editorEditMode = false;
        long long editingSubjectId = 0;


        // =========================================================
        // SUBJECT NAME / CODE NORMALIZATION
        // =========================================================

        String^ NormalizeSubjectName(
            String^ value)
        {
            if (String::IsNullOrWhiteSpace(value))
            {
                return L"";
            }

            String^ trimmed =
                value->Trim();

            for (int i = 0; i < trimmed->Length; i++)
            {
                if (Char::IsLetter(trimmed[i]))
                {
                    return
                        trimmed->Substring(0, i) +
                        Char::ToUpper(trimmed[i]) +
                        trimmed->Substring(i + 1);
                }
            }

            return trimmed;
        }


        String^ NormalizeSubjectCode(
            String^ value)
        {
            if (String::IsNullOrWhiteSpace(value))
            {
                return L"";
            }

            return
                value->Trim()->ToUpperInvariant();
        }


        // =========================================================
        // PROFILE HELPER
        // =========================================================

        void AddProfileField(
            TableLayoutPanel^ layout,
            int row,
            String^ labelText,
            String^ valueText)
        {
            System::Windows::Forms::Label^ label =
                gcnew Label();

            label->Text =
                labelText;

            label->Dock =
                DockStyle::Fill;

            label->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.5F,
                    FontStyle::Bold
                );

            label->ForeColor =
                Color::FromArgb(
                    71,
                    85,
                    105
                );

            label->TextAlign =
                ContentAlignment::MiddleLeft;

            label->Margin =
                System::Windows::Forms::Padding(
                    0,
                    3,
                    12,
                    3
                );


            System::Windows::Forms::Label^ value =
                gcnew Label();

            value->Text =
                String::IsNullOrWhiteSpace(valueText)
                ? L"Not provided"
                : valueText;

            value->Dock =
                DockStyle::Fill;

            value->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.5F,
                    FontStyle::Regular
                );

            value->ForeColor =
                Color::FromArgb(
                    30,
                    41,
                    59
                );

            value->Margin =
                System::Windows::Forms::Padding(
                    0,
                    3,
                    0,
                    3
                );

            value->TextAlign =
                ContentAlignment::MiddleLeft;

            layout->Controls->Add(
                label,
                0,
                row
            );

            layout->Controls->Add(
                value,
                1,
                row
            );
        }


        void AddEditorLabel(
            TableLayoutPanel^ layout,
            String^ text,
            int row)
        {
            System::Windows::Forms::Label^ label =
                gcnew Label();

            label->Text =
                text;

            label->Dock =
                DockStyle::Fill;

            label->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.5F,
                    FontStyle::Bold
                );

            label->ForeColor =
                Color::FromArgb(
                    71,
                    85,
                    105
                );

            label->TextAlign =
                ContentAlignment::MiddleLeft;

            label->Margin =
                System::Windows::Forms::Padding(
                    0,
                    5,
                    12,
                    5
                );

            layout->Controls->Add(
                label,
                0,
                row
            );
        }


        void AddEditorTextBox(
            TableLayoutPanel^ layout,
            TextBox^ box,
            int row)
        {
            box->Dock =
                DockStyle::Fill;

            box->Margin =
                System::Windows::Forms::Padding(
                    0,
                    5,
                    0,
                    5
                );

            layout->Controls->Add(
                box,
                1,
                row
            );
        }


        // =========================================================
        // LOAD SUBJECTS
        // =========================================================

        void LoadSubjects(
            String^ searchText)
        {
            this->subjectsGrid->Rows->Clear();

            int activeCount = 0;

            try
            {
                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement>
                    stmt;

                if (
                    String::IsNullOrWhiteSpace(
                        searchText
                    )
                )
                {
                    stmt.reset(
                        con->prepareStatement(
                            "SELECT "
                            "s.subject_id, "
                            "s.subject_code, "
                            "s.subject_name, "
                            "s.description, "
                            "s.status, "
                            "(SELECT COUNT(*) "
                            " FROM class_subjects cs "
                            " WHERE cs.subject_id = s.subject_id) "
                            "AS class_count, "
                            "(SELECT COUNT(*) "
                            " FROM examination_subjects es "
                            " WHERE es.subject_id = s.subject_id) "
                            "AS examination_count, "
                            "(SELECT COUNT(*) "
                            " FROM timetable_entries te "
                            " WHERE te.subject_id = s.subject_id) "
                            "AS timetable_count "
                            "FROM subjects s "
                            "ORDER BY s.subject_name ASC"
                        )
                    );
                }
                else
                {
                    stmt.reset(
                        con->prepareStatement(
                            "SELECT "
                            "s.subject_id, "
                            "s.subject_code, "
                            "s.subject_name, "
                            "s.description, "
                            "s.status, "
                            "(SELECT COUNT(*) "
                            " FROM class_subjects cs "
                            " WHERE cs.subject_id = s.subject_id) "
                            "AS class_count, "
                            "(SELECT COUNT(*) "
                            " FROM examination_subjects es "
                            " WHERE es.subject_id = s.subject_id) "
                            "AS examination_count, "
                            "(SELECT COUNT(*) "
                            " FROM timetable_entries te "
                            " WHERE te.subject_id = s.subject_id) "
                            "AS timetable_count "
                            "FROM subjects s "
                            "WHERE s.subject_code LIKE ? "
                            "OR s.subject_name LIKE ? "
                            "OR s.description LIKE ? "
                            "ORDER BY s.subject_name ASC"
                        )
                    );

                    std::string pattern =
                        msclr::interop::marshal_as<std::string>(
                            "%" +
                            searchText +
                            "%"
                        );

                    stmt->setString(
                        1,
                        pattern
                    );

                    stmt->setString(
                        2,
                        pattern
                    );

                    stmt->setString(
                        3,
                        pattern
                    );
                }


                std::unique_ptr<sql::ResultSet>
                    result(
                        stmt->executeQuery()
                    );

                while (result->next())
                {
                    long long subjectId =
                        result->getInt64(
                            "subject_id"
                        );

                    String^ subjectCode =
                        gcnew String(
                            result->getString(
                                "subject_code"
                            ).c_str()
                        );

                    String^ subjectName =
                        gcnew String(
                            result->getString(
                                "subject_name"
                            ).c_str()
                        );

                    String^ description =
                        result->isNull(
                            "description"
                        )
                        ? L""
                        : gcnew String(
                            result->getString(
                                "description"
                            ).c_str()
                        );

                    String^ status =
                        gcnew String(
                            result->getString(
                                "status"
                            ).c_str()
                        );

                    int classCount =
                        result->getInt(
                            "class_count"
                        );

                    int examinationCount =
                        result->getInt(
                            "examination_count"
                        );

                    int timetableCount =
                        result->getInt(
                            "timetable_count"
                        );

                    this->subjectsGrid->Rows->Add(
                        subjectId,
                        subjectCode,
                        subjectName,
                        description,
                        status,
                        classCount,
                        examinationCount,
                        timetableCount
                    );

                    if (
                        status->Equals(
                            L"Active",
                            StringComparison::OrdinalIgnoreCase
                        )
                    )
                    {
                        activeCount++;
                    }
                }

                this->lblSubjectCount->Text =
                    L"Active Subjects: " +
                    activeCount.ToString();
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


        // =========================================================
        // SEARCH
        // =========================================================

        System::Void btnSearch_Click(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            LoadSubjects(
                this->txtSearch->Text->Trim()
            );
        }


        System::Void txtSearch_KeyDown(
            System::Object^ sender,
            KeyEventArgs^ e)
        {
            if (
                e->KeyCode ==
                Keys::Enter
            )
            {
                e->SuppressKeyPress =
                    true;

                LoadSubjects(
                    this->txtSearch->Text->Trim()
                );
            }
        }


        System::Void subjectsGrid_SelectionChanged(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            bool hasSelection =
                this->subjectsGrid->SelectedRows->Count >
                0;

            this->btnViewProfile->Enabled =
                hasSelection;

            this->btnEditSubject->Enabled =
                hasSelection;

            this->btnToggleSubject->Enabled =
                hasSelection;
        }


        // =========================================================
        // OPEN EDITOR
        // =========================================================

        void OpenSubjectEditor(
            long long subjectId,
            bool editMode)
        {
            this->editorEditMode =
                editMode;

            this->editingSubjectId =
                subjectId;

            this->editorForm =
                gcnew Form();

            this->editorForm->Text =
                editMode
                ? L"Edit Subject"
                : L"Register Subject";

            this->editorForm->StartPosition =
                FormStartPosition::CenterParent;

            this->editorForm->FormBorderStyle =
                System::Windows::Forms::FormBorderStyle::FixedSingle;

            this->editorForm->MaximizeBox =
                false;

            this->editorForm->MinimizeBox =
                false;

            this->editorForm->ShowInTaskbar =
                false;

            this->editorForm->ClientSize =
                System::Drawing::Size(
                    650,
                    520
                );

            this->editorForm->BackColor =
                Color::FromArgb(
                    248,
                    250,
                    252
                );


            Panel^ header =
                gcnew Panel();

            header->Dock =
                DockStyle::Top;

            header->Height =
                90;

            header->BackColor =
                Color::FromArgb(
                    30,
                    41,
                    59
                );

            header->Padding =
                System::Windows::Forms::Padding(
                    20,
                    12,
                    20,
                    10
                );


            System::Windows::Forms::Label^ title =
                gcnew Label();

            title->Text =
                editMode
                ? L"Edit Subject"
                : L"Register Subject";

            title->Dock =
                DockStyle::Top;

            title->Height =
                36;

            title->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    18.0F,
                    FontStyle::Bold
                );

            title->ForeColor =
                Color::White;


            System::Windows::Forms::Label^ subtitle =
                gcnew Label();

            subtitle->Text =
                editMode
                ? L"Update the subject information."
                : L"Enter the subject information.";

            subtitle->Dock =
                DockStyle::Fill;

            subtitle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.5F
                );

            subtitle->ForeColor =
                Color::Gainsboro;


            header->Controls->Add(
                subtitle
            );

            header->Controls->Add(
                title
            );


            TableLayoutPanel^ layout =
                gcnew TableLayoutPanel();

            layout->Dock =
                DockStyle::Fill;

            layout->Padding =
                System::Windows::Forms::Padding(
                    24,
                    20,
                    24,
                    15
                );

            layout->ColumnCount =
                2;

            layout->RowCount =
                3;

            layout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Absolute,
                    145.0F
                )
            );

            layout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    100.0F
                )
            );

            for (int i = 0; i < 3; i++)
            {
                layout->RowStyles->Add(
                    gcnew RowStyle(
                        SizeType::Absolute,
                        i == 2
                        ? 130.0F
                        : 44.0F
                    )
                );
            }


            this->txtSubjectCode =
                gcnew TextBox();

            this->txtSubjectName =
                gcnew TextBox();

            this->txtDescription =
                gcnew TextBox();

            AddEditorLabel(
                layout,
                L"Subject Code",
                0
            );

            AddEditorTextBox(
                layout,
                this->txtSubjectCode,
                0
            );


            AddEditorLabel(
                layout,
                L"Subject Name",
                1
            );

            AddEditorTextBox(
                layout,
                this->txtSubjectName,
                1
            );


            AddEditorLabel(
                layout,
                L"Description",
                2
            );

            this->txtDescription->Multiline =
                true;

            this->txtDescription->ScrollBars =
                ScrollBars::Vertical;

            this->txtDescription->Dock =
                DockStyle::Fill;

            this->txtDescription->Margin =
                System::Windows::Forms::Padding(
                    0,
                    5,
                    0,
                    5
                );

            layout->Controls->Add(
                this->txtDescription,
                1,
                2
            );


            Panel^ footer =
                gcnew Panel();

            footer->Dock =
                DockStyle::Bottom;

            footer->Height =
                65;

            footer->Padding =
                System::Windows::Forms::Padding(
                    24,
                    10,
                    24,
                    10
                );


            this->btnEditorCancel =
                gcnew Button();

            this->btnEditorCancel->Text =
                L"Cancel";

            this->btnEditorCancel->Dock =
                DockStyle::Right;

            this->btnEditorCancel->Width =
                110;

            this->btnEditorCancel->Height =
                38;

            this->btnEditorCancel->BackColor =
                Color::FromArgb(
                    226,
                    232,
                    240
                );

            this->btnEditorCancel->ForeColor =
                Color::FromArgb(
                    30,
                    41,
                    59
                );

            this->btnEditorCancel->FlatStyle =
                FlatStyle::Flat;

            this->btnEditorCancel->FlatAppearance->BorderSize =
                0;


            this->btnEditorSave =
                gcnew Button();

            this->btnEditorSave->Text =
                editMode
                ? L"Save Changes"
                : L"Register Subject";

            this->btnEditorSave->Dock =
                DockStyle::Right;

            this->btnEditorSave->Width =
                150;

            this->btnEditorSave->Height =
                38;

            this->btnEditorSave->Margin =
                System::Windows::Forms::Padding(
                    0,
                    0,
                    10,
                    0
                );

            this->btnEditorSave->BackColor =
                Color::FromArgb(
                    38,
                    117,
                    92
                );

            this->btnEditorSave->ForeColor =
                Color::White;

            this->btnEditorSave->FlatStyle =
                FlatStyle::Flat;

            this->btnEditorSave->FlatAppearance->BorderSize =
                0;


            footer->Controls->Add(
                this->btnEditorCancel
            );

            footer->Controls->Add(
                this->btnEditorSave
            );


            this->btnEditorCancel->Click +=
                gcnew System::EventHandler(
                    this,
                    &SubjectManagement::btnEditorCancel_Click
                );

            this->btnEditorSave->Click +=
                gcnew System::EventHandler(
                    this,
                    &SubjectManagement::btnEditorSave_Click
                );


            this->editorForm->Controls->Add(
                layout
            );

            this->editorForm->Controls->Add(
                footer
            );

            this->editorForm->Controls->Add(
                header
            );


            if (editMode)
            {
                LoadSubjectForEdit(
                    subjectId
                );
            }

            this->editorForm->ShowDialog(
                this
            );
        }


        // =========================================================
        // LOAD SUBJECT FOR EDIT
        // =========================================================

        void LoadSubjectForEdit(
            long long subjectId)
        {
            try
            {
                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement>
                    stmt(
                        con->prepareStatement(
                            "SELECT "
                            "subject_code, "
                            "subject_name, "
                            "description "
                            "FROM subjects "
                            "WHERE subject_id = ? "
                            "LIMIT 1"
                        )
                    );

                stmt->setInt64(
                    1,
                    subjectId
                );

                std::unique_ptr<sql::ResultSet>
                    result(
                        stmt->executeQuery()
                    );

                if (!result->next())
                {
                    MessageBox::Show(
                        L"Subject record could not be found.",
                        L"Edit Subject",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Warning
                    );

                    this->editorForm->Close();
                    return;
                }

                this->txtSubjectCode->Text =
                    gcnew String(
                        result->getString(
                            "subject_code"
                        ).c_str()
                    );

                this->txtSubjectName->Text =
                    gcnew String(
                        result->getString(
                            "subject_name"
                        ).c_str()
                    );

                if (
                    !result->isNull(
                        "description"
                    )
                )
                {
                    this->txtDescription->Text =
                        gcnew String(
                            result->getString(
                                "description"
                            ).c_str()
                        );
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

                this->editorForm->Close();
            }
        }


        // =========================================================
        // SAVE / UPDATE SUBJECT
        // =========================================================

        System::Void btnEditorSave_Click(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            if (
                String::IsNullOrWhiteSpace(
                    this->txtSubjectCode->Text
                )
            )
            {
                MessageBox::Show(
                    L"Please enter the subject code.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                this->txtSubjectCode->Focus();
                return;
            }

            if (
                String::IsNullOrWhiteSpace(
                    this->txtSubjectName->Text
                )
            )
            {
                MessageBox::Show(
                    L"Please enter the subject name.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                this->txtSubjectName->Focus();
                return;
            }

            // Normalize the naming convention before validation
            // and database storage.
            this->txtSubjectCode->Text =
                NormalizeSubjectCode(
                    this->txtSubjectCode->Text
                );

            this->txtSubjectName->Text =
                NormalizeSubjectName(
                    this->txtSubjectName->Text
                );

            try
            {
                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement>
                    checkStmt(
                        con->prepareStatement(
                            editorEditMode
                            ? "SELECT subject_id "
                              "FROM subjects "
                              "WHERE (subject_code = ? "
                              "OR subject_name = ?) "
                              "AND subject_id <> ? "
                              "LIMIT 1"
                            : "SELECT subject_id "
                              "FROM subjects "
                              "WHERE subject_code = ? "
                              "OR subject_name = ? "
                              "LIMIT 1"
                        )
                    );

                std::string subjectCode =
                    msclr::interop::marshal_as<std::string>(
                        this->txtSubjectCode->Text->Trim()
                    );

                std::string subjectName =
                    msclr::interop::marshal_as<std::string>(
                        this->txtSubjectName->Text->Trim()
                    );

                checkStmt->setString(
                    1,
                    subjectCode
                );

                checkStmt->setString(
                    2,
                    subjectName
                );

                if (editorEditMode)
                {
                    checkStmt->setInt64(
                        3,
                        this->editingSubjectId
                    );
                }

                std::unique_ptr<sql::ResultSet>
                    checkResult(
                        checkStmt->executeQuery()
                    );

                if (checkResult->next())
                {
                    MessageBox::Show(
                        L"That subject code or subject name is already in use.",
                        L"Validation",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Warning
                    );

                    return;
                }


                std::string description =
                    msclr::interop::marshal_as<std::string>(
                        this->txtDescription->Text->Trim()
                    );


                if (editorEditMode)
                {
                    std::unique_ptr<sql::PreparedStatement>
                        stmt(
                            con->prepareStatement(
                                "UPDATE subjects "
                                "SET subject_code = ?, "
                                "subject_name = ?, "
                                "description = ? "
                                "WHERE subject_id = ?"
                            )
                        );

                    stmt->setString(
                        1,
                        subjectCode
                    );

                    stmt->setString(
                        2,
                        subjectName
                    );

                    stmt->setString(
                        3,
                        description
                    );

                    stmt->setInt64(
                        4,
                        this->editingSubjectId
                    );

                    stmt->executeUpdate();

                    MessageBox::Show(
                        L"Subject details updated successfully.",
                        L"Subjects",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Information
                    );
                }
                else
                {
                    std::unique_ptr<sql::PreparedStatement>
                        stmt(
                            con->prepareStatement(
                                "INSERT INTO subjects "
                                "("
                                "subject_code, "
                                "subject_name, "
                                "description, "
                                "status"
                                ") "
                                "VALUES (?, ?, ?, 'Active')"
                            )
                        );

                    stmt->setString(
                        1,
                        subjectCode
                    );

                    stmt->setString(
                        2,
                        subjectName
                    );

                    stmt->setString(
                        3,
                        description
                    );

                    stmt->executeUpdate();

                    MessageBox::Show(
                        L"Subject registered successfully.",
                        L"Subjects",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Information
                    );
                }

                this->editorForm->DialogResult =
                    System::Windows::Forms::DialogResult::OK;

                this->editorForm->Close();
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


        System::Void btnEditorCancel_Click(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            this->editorForm->Close();
        }


        // =========================================================
        // REGISTER SUBJECT
        // =========================================================

        System::Void btnRegisterSubject_Click(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            OpenSubjectEditor(
                0,
                false
            );

            if (
                this->editorForm->DialogResult ==
                System::Windows::Forms::DialogResult::OK
            )
            {
                LoadSubjects(
                    this->txtSearch->Text->Trim()
                );
            }
        }


        // =========================================================
        // VIEW PROFILE
        // =========================================================

        System::Void btnViewProfile_Click(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            if (
                this->subjectsGrid->SelectedRows->Count ==
                0
            )
            {
                MessageBox::Show(
                    L"Please select a subject.",
                    L"Subjects",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                return;
            }

            long long subjectId =
                Convert::ToInt64(
                    this->subjectsGrid
                        ->SelectedRows[0]
                        ->Cells["SubjectId"]
                        ->Value
                );

            try
            {
                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement>
                    stmt(
                        con->prepareStatement(
                            "SELECT "
                            "s.subject_code, "
                            "s.subject_name, "
                            "s.description, "
                            "s.status, "
                            "s.created_at, "
                            "s.updated_at, "
                            "(SELECT COUNT(*) "
                            " FROM class_subjects cs "
                            " WHERE cs.subject_id = s.subject_id) "
                            "AS class_count, "
                            "(SELECT COUNT(*) "
                            " FROM examination_subjects es "
                            " WHERE es.subject_id = s.subject_id) "
                            "AS examination_count, "
                            "(SELECT COUNT(*) "
                            " FROM timetable_entries te "
                            " WHERE te.subject_id = s.subject_id) "
                            "AS timetable_count "
                            "FROM subjects s "
                            "WHERE s.subject_id = ? "
                            "LIMIT 1"
                        )
                    );

                stmt->setInt64(
                    1,
                    subjectId
                );

                std::unique_ptr<sql::ResultSet>
                    result(
                        stmt->executeQuery()
                    );

                if (!result->next())
                {
                    MessageBox::Show(
                        L"Subject record could not be found.",
                        L"Subjects",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Warning
                    );

                    return;
                }


                String^ subjectCode =
                    gcnew String(
                        result->getString(
                            "subject_code"
                        ).c_str()
                    );

                String^ subjectName =
                    gcnew String(
                        result->getString(
                            "subject_name"
                        ).c_str()
                    );

                String^ description =
                    result->isNull("description")
                    ? L""
                    : gcnew String(
                        result->getString(
                            "description"
                        ).c_str()
                    );

                String^ status =
                    gcnew String(
                        result->getString(
                            "status"
                        ).c_str()
                    );

                String^ createdAt =
                    result->isNull("created_at")
                    ? L"Not recorded"
                    : gcnew String(
                        result->getString(
                            "created_at"
                        ).c_str()
                    );

                String^ updatedAt =
                    result->isNull("updated_at")
                    ? L"Not recorded"
                    : gcnew String(
                        result->getString(
                            "updated_at"
                        ).c_str()
                    );

                String^ classCount =
                    Convert::ToString(
                        result->getInt(
                            "class_count"
                        )
                    );

                String^ examinationCount =
                    Convert::ToString(
                        result->getInt(
                            "examination_count"
                        )
                    );

                String^ timetableCount =
                    Convert::ToString(
                        result->getInt(
                            "timetable_count"
                        )
                    );


                Form^ profileForm =
                    gcnew Form();

                profileForm->Text =
                    L"Subject Profile";

                profileForm->StartPosition =
                    FormStartPosition::CenterScreen;

                profileForm->FormBorderStyle =
                    System::Windows::Forms::FormBorderStyle::FixedSingle;

                profileForm->MaximizeBox =
                    false;

                profileForm->MinimizeBox =
                    false;

                profileForm->ShowInTaskbar =
                    false;

                profileForm->ClientSize =
                    System::Drawing::Size(
                        720,
                        620
                    );

                profileForm->BackColor =
                    Color::FromArgb(
                        248,
                        250,
                        252
                    );


                Panel^ profileHeader =
                    gcnew Panel();

                profileHeader->Dock =
                    DockStyle::Top;

                profileHeader->Height =
                    140;

                profileHeader->BackColor =
                    Color::FromArgb(
                        30,
                        41,
                        59
                    );

                profileHeader->Padding =
                    System::Windows::Forms::Padding(
                        24,
                        16,
                        24,
                        12
                    );


                System::Windows::Forms::Label^ nameLabel =
                    gcnew Label();

                nameLabel->Text =
                    subjectName;

                nameLabel->Dock =
                    DockStyle::Top;

                nameLabel->Height =
                    46;

                nameLabel->Font =
                    gcnew System::Drawing::Font(
                        L"Segoe UI Semibold",
                        20.0F,
                        FontStyle::Bold
                    );

                nameLabel->ForeColor =
                    Color::White;

                nameLabel->TextAlign =
                    ContentAlignment::MiddleLeft;


                System::Windows::Forms::Label^ codeLabel =
                    gcnew Label();

                codeLabel->Text =
                    L"Subject Code  " +
                    subjectCode;

                codeLabel->Dock =
                    DockStyle::Top;

                codeLabel->Height =
                    30;

                codeLabel->Font =
                    gcnew System::Drawing::Font(
                        L"Segoe UI",
                        10.0F
                    );

                codeLabel->ForeColor =
                    Color::Gainsboro;


                System::Windows::Forms::Label^ statusLabel =
                    gcnew Label();

                statusLabel->Text =
                    L"Status: " +
                    status;

                statusLabel->Dock =
                    DockStyle::Top;

                statusLabel->Height =
                    30;

                statusLabel->Font =
                    gcnew System::Drawing::Font(
                        L"Segoe UI Semibold",
                        9.0F,
                        FontStyle::Bold
                    );

                statusLabel->ForeColor =
                    status->Equals(
                        L"Active",
                        StringComparison::OrdinalIgnoreCase
                    )
                    ? Color::FromArgb(
                        167,
                        243,
                        208
                    )
                    : Color::FromArgb(
                        254,
                        202,
                        202
                    );


                profileHeader->Controls->Add(
                    statusLabel
                );

                profileHeader->Controls->Add(
                    codeLabel
                );

                profileHeader->Controls->Add(
                    nameLabel
                );


                Panel^ content =
                    gcnew Panel();

                content->Dock =
                    DockStyle::Fill;

                content->AutoScroll =
                    true;

                content->HorizontalScroll->Visible =
                    false;

                content->VerticalScroll->Visible =
                    true;

                content->Padding =
                    System::Windows::Forms::Padding(
                        20
                    );


                TableLayoutPanel^ contentLayout =
                    gcnew TableLayoutPanel();

                contentLayout->Dock =
                    DockStyle::Top;

                contentLayout->AutoSize =
                    true;

                contentLayout->ColumnCount =
                    1;

                contentLayout->RowCount =
                    1;

                contentLayout->ColumnStyles->Add(
                    gcnew ColumnStyle(
                        SizeType::Percent,
                        100.0F
                    )
                );


                GroupBox^ info =
                    gcnew GroupBox();

                info->Text =
                    L"Subject Information";

                info->Dock =
                    DockStyle::Top;

                info->Height =
                    410;

                info->Padding =
                    System::Windows::Forms::Padding(
                        14,
                        18,
                        14,
                        10
                    );


                TableLayoutPanel^ infoLayout =
                    gcnew TableLayoutPanel();

                infoLayout->Dock =
                    DockStyle::Fill;

                infoLayout->ColumnCount =
                    2;

                infoLayout->RowCount =
                    9;

                infoLayout->ColumnStyles->Add(
                    gcnew ColumnStyle(
                        SizeType::Absolute,
                        170.0F
                    )
                );

                infoLayout->ColumnStyles->Add(
                    gcnew ColumnStyle(
                        SizeType::Percent,
                        100.0F
                    )
                );


                for (int i = 0; i < 9; i++)
                {
                    float rowHeight =
                        (i == 2)
                        ? 55.0F
                        : 38.0F;

                    infoLayout->RowStyles->Add(
                        gcnew RowStyle(
                            SizeType::Absolute,
                            rowHeight
                        )
                    );
                }


                AddProfileField(
                    infoLayout,
                    0,
                    L"Subject Code",
                    subjectCode
                );

                AddProfileField(
                    infoLayout,
                    1,
                    L"Subject Name",
                    subjectName
                );

                AddProfileField(
                    infoLayout,
                    2,
                    L"Description",
                    description
                );

                AddProfileField(
                    infoLayout,
                    3,
                    L"Status",
                    status
                );

                AddProfileField(
                    infoLayout,
                    4,
                    L"Classes Assigned",
                    classCount
                );

                AddProfileField(
                    infoLayout,
                    5,
                    L"Examination Links",
                    examinationCount
                );

                AddProfileField(
                    infoLayout,
                    6,
                    L"Timetable Entries",
                    timetableCount
                );

                AddProfileField(
                    infoLayout,
                    7,
                    L"Created",
                    createdAt
                );

                AddProfileField(
                    infoLayout,
                    8,
                    L"Last Updated",
                    updatedAt
                );


                info->Controls->Add(
                    infoLayout
                );

                contentLayout->Controls->Add(
                    info,
                    0,
                    0
                );

                content->Controls->Add(
                    contentLayout
                );


                Panel^ footer =
                    gcnew Panel();

                footer->Dock =
                    DockStyle::Bottom;

                footer->Height =
                    60;

                footer->Padding =
                    System::Windows::Forms::Padding(
                        20,
                        8,
                        20,
                        10
                    );


                Button^ closeButton =
                    gcnew Button();

                closeButton->Text =
                    L"Close";

                closeButton->Dock =
                    DockStyle::Right;

                closeButton->Width =
                    110;

                closeButton->Height =
                    38;

                closeButton->BackColor =
                    Color::FromArgb(
                        30,
                        41,
                        59
                    );

                closeButton->ForeColor =
                    Color::White;

                closeButton->FlatStyle =
                    FlatStyle::Flat;

                closeButton->FlatAppearance->BorderSize =
                    0;

                closeButton->DialogResult =
                    System::Windows::Forms::DialogResult::Cancel;


                footer->Controls->Add(
                    closeButton
                );


                profileForm->Controls->Add(
                    content
                );

                profileForm->Controls->Add(
                    footer
                );

                profileForm->Controls->Add(
                    profileHeader
                );

                profileForm->ShowDialog(
                    this
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


        // =========================================================
        // EDIT
        // =========================================================

        System::Void btnEditSubject_Click(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            if (
                this->subjectsGrid->SelectedRows->Count ==
                0
            )
            {
                MessageBox::Show(
                    L"Please select a subject.",
                    L"Subjects",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                return;
            }

            long long subjectId =
                Convert::ToInt64(
                    this->subjectsGrid
                        ->SelectedRows[0]
                        ->Cells["SubjectId"]
                        ->Value
                );

            OpenSubjectEditor(
                subjectId,
                true
            );

            if (
                this->editorForm->DialogResult ==
                System::Windows::Forms::DialogResult::OK
            )
            {
                LoadSubjects(
                    this->txtSearch->Text->Trim()
                );
            }
        }


        // =========================================================
        // TOGGLE STATUS
        // =========================================================

        System::Void btnToggleSubject_Click(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            if (
                this->subjectsGrid->SelectedRows->Count ==
                0
            )
            {
                MessageBox::Show(
                    L"Please select a subject.",
                    L"Subjects",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                return;
            }

            long long subjectId =
                Convert::ToInt64(
                    this->subjectsGrid
                        ->SelectedRows[0]
                        ->Cells["SubjectId"]
                        ->Value
                );

            String^ currentStatus =
                Convert::ToString(
                    this->subjectsGrid
                        ->SelectedRows[0]
                        ->Cells["Status"]
                        ->Value
                );

            bool isActive =
                currentStatus->Equals(
                    L"Active",
                    StringComparison::OrdinalIgnoreCase
                );

            String^ newStatus =
                isActive
                ? L"Inactive"
                : L"Active";

            String^ action =
                isActive
                ? L"deactivate"
                : L"activate";

            if (
                MessageBox::Show(
                    L"Are you sure you want to " +
                    action +
                    L" this subject?",
                    L"Subjects",
                    MessageBoxButtons::YesNo,
                    MessageBoxIcon::Question
                )
                !=
                System::Windows::Forms::DialogResult::Yes
            )
            {
                return;
            }

            try
            {
                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement>
                    stmt(
                        con->prepareStatement(
                            "UPDATE subjects "
                            "SET status = ? "
                            "WHERE subject_id = ?"
                        )
                    );

                stmt->setString(
                    1,
                    msclr::interop::marshal_as<std::string>(
                        newStatus
                    )
                );

                stmt->setInt64(
                    2,
                    subjectId
                );

                stmt->executeUpdate();

                LoadSubjects(
                    this->txtSearch->Text->Trim()
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


        // =========================================================
        // BACK
        // =========================================================

        System::Void btnBack_Click(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            this->Close();
        }


        // =========================================================
        // INITIALIZE COMPONENTS
        // =========================================================

        void InitializeComponent()
        {
            this->SuspendLayout();

            this->components =
                gcnew System::ComponentModel::Container();

            this->mainLayout =
                gcnew TableLayoutPanel();

            this->headerPanel =
                gcnew Panel();

            this->lblTitle =
                gcnew Label();

            this->lblSubtitle =
                gcnew Label();

            this->actionPanel =
                gcnew Panel();

            this->btnRegisterSubject =
                gcnew Button();

            this->lblSearch =
                gcnew Label();

            this->txtSearch =
                gcnew TextBox();

            this->btnSearch =
                gcnew Button();

            this->lblSubjectCount =
                gcnew Label();

            this->subjectsGrid =
                gcnew DataGridView();

            this->buttonPanel =
                gcnew FlowLayoutPanel();

            this->btnViewProfile =
                gcnew Button();

            this->btnEditSubject =
                gcnew Button();

            this->btnToggleSubject =
                gcnew Button();

            this->btnBack =
                gcnew Button();


            // =====================================================
            // FORM
            // =====================================================

            this->Text =
                L"Subjects";

            this->StartPosition =
                FormStartPosition::CenterScreen;

            this->WindowState =
                FormWindowState::Maximized;

            this->FormBorderStyle =
                System::Windows::Forms::FormBorderStyle::Sizable;

            this->MaximizeBox =
                true;

            this->MinimizeBox =
                true;

            this->MinimumSize =
                System::Drawing::Size(
                    980,
                    620
                );

            this->ClientSize =
                System::Drawing::Size(
                    1120,
                    760
                );

            this->BackColor =
                Color::FromArgb(
                    248,
                    250,
                    252
                );


            // =====================================================
            // MAIN LAYOUT
            // =====================================================

            this->mainLayout->Dock =
                DockStyle::Fill;

            this->mainLayout->Padding =
                System::Windows::Forms::Padding(
                    20
                );

            this->mainLayout->ColumnCount =
                1;

            this->mainLayout->RowCount =
                5;

            this->mainLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    100.0F
                )
            );

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    86.0F
                )
            );

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    70.0F
                )
            );

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    38.0F
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
                    58.0F
                )
            );


            // =====================================================
            // HEADER
            // =====================================================

            this->headerPanel->Dock =
                DockStyle::Fill;

            this->headerPanel->Margin =
                System::Windows::Forms::Padding(
                    0,
                    0,
                    0,
                    12
                );

            this->headerPanel->BackColor =
                Color::FromArgb(
                    30,
                    41,
                    59
                );

            this->headerPanel->Padding =
                System::Windows::Forms::Padding(
                    20,
                    8,
                    20,
                    8
                );


            this->lblTitle->AutoSize =
                false;

            this->lblTitle->Dock =
                DockStyle::Top;

            this->lblTitle->Height =
                44;

            this->lblTitle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    16,
                    FontStyle::Bold
                );

            this->lblTitle->ForeColor =
                Color::White;

            this->lblTitle->Text =
                L"Subjects";

            this->lblTitle->TextAlign =
                ContentAlignment::MiddleLeft;


            this->lblSubtitle->AutoSize =
                false;

            this->lblSubtitle->Dock =
                DockStyle::Fill;

            this->lblSubtitle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.5F,
                    System::Drawing::FontStyle::Regular
                );

            this->lblSubtitle->ForeColor =
                Color::Gainsboro;

            this->lblSubtitle->Text =
                L"Register, search and manage school subjects.";

            this->lblSubtitle->TextAlign =
                ContentAlignment::TopLeft;


            this->headerPanel->Controls->Add(
                this->lblSubtitle
            );

            this->headerPanel->Controls->Add(
                this->lblTitle
            );


            // =====================================================
            // ACTION PANEL
            // =====================================================

            this->actionPanel->Dock =
                DockStyle::Fill;

            this->actionPanel->Margin =
                System::Windows::Forms::Padding(
                    0,
                    0,
                    0,
                    10
                );

            this->actionPanel->BackColor =
                Color::White;

            this->actionPanel->BorderStyle =
                BorderStyle::FixedSingle;


            this->btnRegisterSubject->Text =
                L"+ Register Subject";

            this->btnRegisterSubject->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    9.5F,
                    FontStyle::Bold
                );

            this->btnRegisterSubject->BackColor =
                Color::FromArgb(
                    30,
                    41,
                    59
                );

            this->btnRegisterSubject->ForeColor =
                Color::White;

            this->btnRegisterSubject->FlatStyle =
                FlatStyle::Flat;

            this->btnRegisterSubject->FlatAppearance->BorderSize =
                0;

            this->btnRegisterSubject->Location =
                System::Drawing::Point(
                    15,
                    13
                );

            this->btnRegisterSubject->Size =
                System::Drawing::Size(
                    170,
                    42
                );


            this->lblSearch->AutoSize =
                true;

            this->lblSearch->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.5F
                );

            this->lblSearch->Text =
                L"Search";

            this->lblSearch->Location =
                System::Drawing::Point(
                    420,
                    24
                );

            this->lblSearch->Anchor =
                AnchorStyles::Top | AnchorStyles::Right;


            this->txtSearch->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.5F
                );

            this->txtSearch->Location =
                System::Drawing::Point(
                    475,
                    19
                );

            this->txtSearch->Anchor =
                AnchorStyles::Top | AnchorStyles::Right;

            this->txtSearch->Size =
                System::Drawing::Size(
                    360,
                    30
                );


            this->btnSearch->Text =
                L"Search";

            this->btnSearch->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.5F
                );

            this->btnSearch->Location =
                System::Drawing::Point(
                    845,
                    18
                );

            this->btnSearch->Anchor =
                AnchorStyles::Top | AnchorStyles::Right;

            this->btnSearch->Size =
                System::Drawing::Size(
                    90,
                    32
                );


            this->actionPanel->Controls->Add(
                this->btnRegisterSubject
            );

            this->actionPanel->Controls->Add(
                this->lblSearch
            );

            this->actionPanel->Controls->Add(
                this->txtSearch
            );

            this->actionPanel->Controls->Add(
                this->btnSearch
            );


            // =====================================================
            // COUNT
            // =====================================================

            this->lblSubjectCount->AutoSize =
                true;

            this->lblSubjectCount->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.5F
                );

            this->lblSubjectCount->ForeColor =
                Color::DimGray;

            this->lblSubjectCount->Text =
                L"Active Subjects: 0";

            this->lblSubjectCount->Dock =
                DockStyle::Fill;

            this->lblSubjectCount->TextAlign =
                ContentAlignment::MiddleLeft;


            // =====================================================
            // GRID
            // =====================================================

            this->subjectsGrid->Dock =
                DockStyle::Fill;

            this->subjectsGrid->Margin =
                System::Windows::Forms::Padding(
                    0,
                    0,
                    0,
                    8
                );

            this->subjectsGrid->AllowUserToAddRows =
                false;

            this->subjectsGrid->AllowUserToDeleteRows =
                false;

            this->subjectsGrid->AllowUserToResizeRows =
                false;

            this->subjectsGrid->AutoGenerateColumns =
                false;

            this->subjectsGrid->BackgroundColor =
                Color::White;

            this->subjectsGrid->BorderStyle =
                BorderStyle::None;

            this->subjectsGrid->CellBorderStyle =
                DataGridViewCellBorderStyle::SingleHorizontal;

            this->subjectsGrid->ColumnHeadersHeight =
                42;

            this->subjectsGrid->EnableHeadersVisualStyles =
                false;

            this->subjectsGrid->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.5F
                );

            this->subjectsGrid->ReadOnly =
                true;

            this->subjectsGrid->RowHeadersVisible =
                false;

            this->subjectsGrid->SelectionMode =
                DataGridViewSelectionMode::FullRowSelect;

            this->subjectsGrid->MultiSelect =
                false;

            this->subjectsGrid->AutoSizeRowsMode =
                DataGridViewAutoSizeRowsMode::None;

            this->subjectsGrid->RowTemplate->Height =
                34;


            this->subjectsGrid
                ->ColumnHeadersDefaultCellStyle
                ->BackColor =
                Color::FromArgb(
                    30,
                    41,
                    59
                );

            this->subjectsGrid
                ->ColumnHeadersDefaultCellStyle
                ->ForeColor =
                Color::White;

            this->subjectsGrid
                ->ColumnHeadersDefaultCellStyle
                ->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    9.5F,
                    FontStyle::Bold
                );

            this->subjectsGrid
                ->ColumnHeadersDefaultCellStyle
                ->Alignment =
                DataGridViewContentAlignment::MiddleLeft;


            DataGridViewTextBoxColumn^ subjectIdColumn =
                gcnew DataGridViewTextBoxColumn();

            subjectIdColumn->HeaderText =
                L"Subject ID";

            subjectIdColumn->Name =
                L"SubjectId";

            subjectIdColumn->Visible =
                false;

            this->subjectsGrid->Columns->Add(
                subjectIdColumn
            );


            DataGridViewTextBoxColumn^ codeColumn =
                gcnew DataGridViewTextBoxColumn();

            codeColumn->HeaderText =
                L"Subject Code";

            codeColumn->Name =
                L"SubjectCode";

            codeColumn->Width =
                130;

            this->subjectsGrid->Columns->Add(
                codeColumn
            );


            DataGridViewTextBoxColumn^ nameColumn =
                gcnew DataGridViewTextBoxColumn();

            nameColumn->HeaderText =
                L"Subject Name";

            nameColumn->Name =
                L"SubjectName";

            nameColumn->AutoSizeMode =
                DataGridViewAutoSizeColumnMode::Fill;

            this->subjectsGrid->Columns->Add(
                nameColumn
            );


            DataGridViewTextBoxColumn^ descriptionColumn =
                gcnew DataGridViewTextBoxColumn();

            descriptionColumn->HeaderText =
                L"Description";

            descriptionColumn->Name =
                L"Description";

            descriptionColumn->Width =
                220;

            this->subjectsGrid->Columns->Add(
                descriptionColumn
            );


            DataGridViewTextBoxColumn^ statusColumn =
                gcnew DataGridViewTextBoxColumn();

            statusColumn->HeaderText =
                L"Status";

            statusColumn->Name =
                L"Status";

            statusColumn->Width =
                90;

            this->subjectsGrid->Columns->Add(
                statusColumn
            );


            DataGridViewTextBoxColumn^ classCountColumn =
                gcnew DataGridViewTextBoxColumn();

            classCountColumn->HeaderText =
                L"Classes";

            classCountColumn->Name =
                L"ClassCount";

            classCountColumn->Width =
                75;

            this->subjectsGrid->Columns->Add(
                classCountColumn
            );


            DataGridViewTextBoxColumn^ examinationCountColumn =
                gcnew DataGridViewTextBoxColumn();

            examinationCountColumn->HeaderText =
                L"Exams";

            examinationCountColumn->Name =
                L"ExaminationCount";

            examinationCountColumn->Width =
                75;

            this->subjectsGrid->Columns->Add(
                examinationCountColumn
            );


            DataGridViewTextBoxColumn^ timetableCountColumn =
                gcnew DataGridViewTextBoxColumn();

            timetableCountColumn->HeaderText =
                L"Timetable";

            timetableCountColumn->Name =
                L"TimetableCount";

            timetableCountColumn->Width =
                85;

            this->subjectsGrid->Columns->Add(
                timetableCountColumn
            );


            // =====================================================
            // BOTTOM BUTTONS
            // =====================================================

            this->buttonPanel->Dock =
                DockStyle::Fill;

            this->buttonPanel->FlowDirection =
                FlowDirection::LeftToRight;

            this->buttonPanel->WrapContents =
                false;

            this->buttonPanel->Padding =
                System::Windows::Forms::Padding(
                    0,
                    5,
                    0,
                    0
                );


            this->btnViewProfile->Text =
                L"View Profile";

            this->btnViewProfile->Enabled =
                false;

            this->btnViewProfile->Size =
                System::Drawing::Size(
                    120,
                    38
                );


            this->btnEditSubject->Text =
                L"Edit Subject";

            this->btnEditSubject->Enabled =
                false;

            this->btnEditSubject->Size =
                System::Drawing::Size(
                    120,
                    38
                );


            this->btnToggleSubject->Text =
                L"Activate / Deactivate";

            this->btnToggleSubject->Enabled =
                false;

            this->btnToggleSubject->Size =
                System::Drawing::Size(
                    165,
                    38
                );


            this->btnBack->Text =
                L"Back to Dashboard";

            this->btnBack->Size =
                System::Drawing::Size(
                    150,
                    38
                );


            this->buttonPanel->Controls->Add(
                this->btnViewProfile
            );

            this->buttonPanel->Controls->Add(
                this->btnEditSubject
            );

            this->buttonPanel->Controls->Add(
                this->btnToggleSubject
            );

            this->buttonPanel->Controls->Add(
                this->btnBack
            );


            // =====================================================
            // ADD CONTROLS
            // =====================================================

            this->mainLayout->Controls->Add(
                this->headerPanel,
                0,
                0
            );

            this->mainLayout->Controls->Add(
                this->actionPanel,
                0,
                1
            );

            this->mainLayout->Controls->Add(
                this->lblSubjectCount,
                0,
                2
            );

            this->mainLayout->Controls->Add(
                this->subjectsGrid,
                0,
                3
            );

            this->mainLayout->Controls->Add(
                this->buttonPanel,
                0,
                4
            );

            this->Controls->Add(
                this->mainLayout
            );


            // =====================================================
            // EVENTS
            // =====================================================

            this->btnRegisterSubject->Click +=
                gcnew System::EventHandler(
                    this,
                    &SubjectManagement::btnRegisterSubject_Click
                );

            this->btnSearch->Click +=
                gcnew System::EventHandler(
                    this,
                    &SubjectManagement::btnSearch_Click
                );

            this->txtSearch->KeyDown +=
                gcnew KeyEventHandler(
                    this,
                    &SubjectManagement::txtSearch_KeyDown
                );

            this->subjectsGrid->SelectionChanged +=
                gcnew System::EventHandler(
                    this,
                    &SubjectManagement::subjectsGrid_SelectionChanged
                );

            this->btnViewProfile->Click +=
                gcnew System::EventHandler(
                    this,
                    &SubjectManagement::btnViewProfile_Click
                );

            this->btnEditSubject->Click +=
                gcnew System::EventHandler(
                    this,
                    &SubjectManagement::btnEditSubject_Click
                );

            this->btnToggleSubject->Click +=
                gcnew System::EventHandler(
                    this,
                    &SubjectManagement::btnToggleSubject_Click
                );

            this->btnBack->Click +=
                gcnew System::EventHandler(
                    this,
                    &SubjectManagement::btnBack_Click
                );


            this->ResumeLayout(
                false
            );
        }


    public:

        SubjectManagement()
        {
            InitializeComponent();
            if (System::ComponentModel::LicenseManager::UsageMode != System::ComponentModel::LicenseUsageMode::Designtime)
            {
                LoadSubjects(L"");
            }
        }


    protected:

        ~SubjectManagement()
        {
            if (this->components)
            {
                delete this->components;
            }
        }
    };
}

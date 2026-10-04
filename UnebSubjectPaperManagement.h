#pragma once

#include "DbConnection.h"
#include "ThemeManager.h"

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
    public ref class UnebSubjectPaperManagement : public System::Windows::Forms::Form
    {
    public:
        ref class Item
        {
        public:
            int Id;
            String^ Name;

            Item(int id, String^ name)
            {
                Id = id;
                Name = name;
            }

            virtual String^ ToString() override
            {
                return Name;
            }
        };

    private:
        System::ComponentModel::Container^ components;

        TableLayoutPanel^ mainLayout;
        Panel^ headerPanel;
        Label^ titleLabel;
        Label^ subtitleLabel;
        FlowLayoutPanel^ actionPanel;
        Button^ btnNewSubject;
        Button^ btnEditSubject;
        Button^ btnManagePapers;
        Button^ btnToggleSubject;
        Label^ lblSearch;
        TextBox^ txtSearch;
        Button^ btnSearch;

        DataGridView^ subjectGrid;
        FlowLayoutPanel^ bottomPanel;
        Button^ btnBack;

        Form^ editorForm;
        ComboBox^ cmbCurriculum;
        ComboBox^ cmbSubject;
        ComboBox^ cmbGroup;
        TextBox^ txtUnebSubjectCode;

        Form^ papersForm;
        DataGridView^ papersGrid;
        TextBox^ txtPaperSearch;
        Button^ btnPaperSearch;
        int selectedCurriculumSubjectId = 0;

        bool editSubjectMode = false;
        int editingCurriculumSubjectId = 0;

        Form^ paperEditorForm;
        TextBox^ txtPaperCode;
        TextBox^ txtPaperNumber;
        TextBox^ txtPaperName;
        ComboBox^ cmbPaperType;
        bool editPaperMode = false;
        int editingPaperId = 0;

        void ConfigureButton(Button^ button, String^ text, int width)
        {
            button->Text = text;
            button->Width = width;
            button->Height = 38;
            button->FlatStyle = FlatStyle::Flat;
            button->FlatAppearance->BorderSize = 0;
            button->Margin = System::Windows::Forms::Padding(0, 0, 10, 0);
            button->Font = gcnew System::Drawing::Font(L"Segoe UI Semibold", 9.5F, FontStyle::Bold);
        }

        Label^ FormLabel(String^ text)
        {
            Label^ label = gcnew Label();
            label->Text = text;
            label->Dock = DockStyle::Fill;
            label->TextAlign = ContentAlignment::MiddleLeft;
            label->Font = gcnew System::Drawing::Font(L"Segoe UI Semibold", 9.5F, FontStyle::Bold);
            return label;
        }

        void AddField(TableLayoutPanel^ layout, String^ labelText, Control^ control, int row)
        {
            layout->Controls->Add(FormLabel(labelText), 0, row);
            control->Dock = DockStyle::Fill;
            control->Margin = System::Windows::Forms::Padding(0, 5, 0, 5);
            layout->Controls->Add(control, 1, row);
        }

        int GetSelectedSubjectRecordId()
        {
            if (subjectGrid->SelectedRows->Count == 0)
                return 0;

            Object^ value = subjectGrid->SelectedRows[0]->Cells["CurriculumSubjectId"]->Value;
            return value == nullptr ? 0 : Convert::ToInt32(value);
        }

        void LoadCurricula(int selectedId)
        {
            cmbCurriculum->Items->Clear();
            cmbCurriculum->Items->Add(L"Select curriculum");

            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT "
                        "c.curriculum_id, "
                        "c.curriculum_name, "
                        "al.level_name "
                        "FROM curricula c "
                        "INNER JOIN academic_levels al "
                        "ON al.academic_level_id = c.academic_level_id "
                        "WHERE c.status = 'Active' "
                        "ORDER BY al.level_name, c.curriculum_name"
                    )
                );

                std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());

                int index = 0;
                int selectedIndex = 0;

                while (result->next())
                {
                    ++index;

                    String^ name =
                        gcnew String(result->getString("curriculum_name").c_str());

                    String^ level =
                        gcnew String(result->getString("level_name").c_str());

                    cmbCurriculum->Items->Add(
                        gcnew Item(
                            result->getInt("curriculum_id"),
                            name + L" (" + level + L")"
                        )
                    );

                    if (result->getInt("curriculum_id") == selectedId)
                        selectedIndex = index;
                }

                cmbCurriculum->SelectedIndex = selectedIndex;
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(gcnew String(ex.what()), L"Database Error",
                    MessageBoxButtons::OK, MessageBoxIcon::Error);
            }
        }

        void LoadSubjects(int selectedId)
        {
            cmbSubject->Items->Clear();
            cmbSubject->Items->Add(L"Select subject");

            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT subject_id, subject_name "
                        "FROM subjects "
                        "WHERE status = 'Active' "
                        "ORDER BY subject_name"
                    )
                );

                std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());

                int index = 0;
                int selectedIndex = 0;

                while (result->next())
                {
                    ++index;

                    int id = result->getInt("subject_id");
                    String^ name =
                        gcnew String(result->getString("subject_name").c_str());

                    cmbSubject->Items->Add(gcnew Item(id, name));

                    if (id == selectedId)
                        selectedIndex = index;
                }

                cmbSubject->SelectedIndex = selectedIndex;
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(gcnew String(ex.what()), L"Database Error",
                    MessageBoxButtons::OK, MessageBoxIcon::Error);
            }
        }

        void LoadSubjectRecords(String^ searchText)
        {
            subjectGrid->Rows->Clear();
            searchText = String::IsNullOrWhiteSpace(searchText) ? L"" : searchText->Trim();

            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT "
                        "cs.curriculum_subject_id, "
                        "c.curriculum_name, "
                        "al.level_name, "
                        "s.subject_name, "
                        "cs.uneb_subject_code, "
                        "cs.subject_group, "
                        "cs.status, "
                        "(SELECT COUNT(*) "
                        " FROM subject_papers sp "
                        " WHERE sp.curriculum_subject_id = cs.curriculum_subject_id "
                        " AND sp.status = 'Active') AS paper_count "
                        "FROM curriculum_subjects cs "
                        "INNER JOIN curricula c "
                        "ON c.curriculum_id = cs.curriculum_id "
                        "INNER JOIN academic_levels al "
                        "ON al.academic_level_id = c.academic_level_id "
                        "INNER JOIN subjects s "
                        "ON s.subject_id = cs.subject_id "
                        "WHERE (? = '' OR c.curriculum_name LIKE ? OR al.level_name LIKE ? "
                        "OR s.subject_name LIKE ? OR cs.uneb_subject_code LIKE ? OR cs.subject_group LIKE ?) "
                        "ORDER BY al.level_name, s.subject_name"
                    )
                );

                std::string rawSearch = msclr::interop::marshal_as<std::string>(searchText);
                std::string pattern = msclr::interop::marshal_as<std::string>(L"%" + searchText + L"%");
                stmt->setString(1, rawSearch);
                stmt->setString(2, pattern);
                stmt->setString(3, pattern);
                stmt->setString(4, pattern);
                stmt->setString(5, pattern);
                stmt->setString(6, pattern);

                std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());

                while (result->next())
                {
                    subjectGrid->Rows->Add(
                        result->getInt("curriculum_subject_id"),
                        gcnew String(result->getString("curriculum_name").c_str()),
                        gcnew String(result->getString("level_name").c_str()),
                        gcnew String(result->getString("subject_group").c_str()),
                        gcnew String(result->getString("subject_name").c_str()),
                        gcnew String(result->getString("uneb_subject_code").c_str()),
                        result->getInt("paper_count"),
                        gcnew String(result->getString("status").c_str())
                    );
                }

                UpdateSubjectActions();
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(gcnew String(ex.what()), L"Database Error",
                    MessageBoxButtons::OK, MessageBoxIcon::Error);
            }
        }

        void UpdateSubjectActions()
        {
            bool selected = subjectGrid->SelectedRows->Count > 0;
            btnEditSubject->Enabled = selected;
            btnManagePapers->Enabled = selected;
            btnToggleSubject->Enabled = selected;
        }

        System::Void SubjectSearchClicked(Object^ sender, EventArgs^ e)
        {
            LoadSubjectRecords(txtSearch == nullptr ? L"" : txtSearch->Text);
        }

        System::Void SubjectSearchKeyDown(Object^ sender, KeyEventArgs^ e)
        {
            if (e->KeyCode == Keys::Enter)
            {
                SubjectSearchClicked(sender, e);
                e->SuppressKeyPress = true;
            }
        }

        void LoadSubjectEditorForEdit(int recordId)
        {
            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT curriculum_id, subject_id, uneb_subject_code, subject_group "
                        "FROM curriculum_subjects "
                        "WHERE curriculum_subject_id = ?"
                    )
                );

                stmt->setInt(1, recordId);

                std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());

                if (!result->next())
                    return;

                int curriculumId = result->getInt("curriculum_id");
                int subjectId = result->getInt("subject_id");

                LoadCurricula(curriculumId);
                LoadSubjects(subjectId);

                txtUnebSubjectCode->Text =
                    gcnew String(result->getString("uneb_subject_code").c_str());

                String^ group =
                    gcnew String(result->getString("subject_group").c_str());

                for (int i = 0; i < cmbGroup->Items->Count; ++i)
                {
                    if (String::Equals(
                            cmbGroup->Items[i]->ToString(),
                            group,
                            StringComparison::OrdinalIgnoreCase))
                    {
                        cmbGroup->SelectedIndex = i;
                        break;
                    }
                }
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(gcnew String(ex.what()), L"Database Error",
                    MessageBoxButtons::OK, MessageBoxIcon::Error);
            }
        }

        void SaveSubjectRecord()
        {
            Item^ curriculum =
                dynamic_cast<Item^>(cmbCurriculum->SelectedItem);

            Item^ subject =
                dynamic_cast<Item^>(cmbSubject->SelectedItem);

            if (curriculum == nullptr || subject == nullptr ||
                cmbCurriculum->SelectedIndex == 0 ||
                cmbSubject->SelectedIndex == 0)
            {
                MessageBox::Show(
                    L"Select a curriculum and a subject.",
                    L"Validation",                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                return;
            }

            if (String::IsNullOrWhiteSpace(txtUnebSubjectCode->Text))
            {
                MessageBox::Show(
                    L"Enter the official UNEB subject code.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                txtUnebSubjectCode->Focus();
                return;
            }

            if (cmbGroup->SelectedIndex < 0)
            {
                MessageBox::Show(
                    L"Select a subject group.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                return;
            }

            try
            {
                auto con = DbConnection::GetConnection();

                std::string code =
                    msclr::interop::marshal_as<std::string>(
                        txtUnebSubjectCode->Text->Trim()->ToUpperInvariant());

                std::string group =
                    msclr::interop::marshal_as<std::string>(
                        cmbGroup->SelectedItem->ToString());

                if (!editSubjectMode)
                {
                    std::unique_ptr<sql::PreparedStatement> check(
                        con->prepareStatement(
                            "SELECT curriculum_subject_id "
                            "FROM curriculum_subjects "
                            "WHERE curriculum_id = ? "
                            "AND subject_id = ? "
                            "LIMIT 1"
                        )
                    );

                    check->setInt(1, curriculum->Id);
                    check->setInt(2, subject->Id);

                    std::unique_ptr<sql::ResultSet> checkResult(check->executeQuery());

                    if (checkResult->next())
                    {
                        MessageBox::Show(
                            L"This subject is already configured for the selected curriculum.",
                            L"Validation",
                            MessageBoxButtons::OK,
                            MessageBoxIcon::Warning
                        );
                        return;
                    }
                }

                std::unique_ptr<sql::PreparedStatement> stmt;

                if (editSubjectMode)
                {
                    stmt.reset(
                        con->prepareStatement(
                            "UPDATE curriculum_subjects "
                            "SET curriculum_id = ?, "
                            "subject_id = ?, "
                            "uneb_subject_code = ?, "
                            "subject_group = ? "
                            "WHERE curriculum_subject_id = ?"
                        )
                    );

                    stmt->setInt(1, curriculum->Id);
                    stmt->setInt(2, subject->Id);
                    stmt->setString(3, code);
                    stmt->setString(4, group);
                    stmt->setInt(5, editingCurriculumSubjectId);
                    stmt->executeUpdate();
                }
                else
                {
                    stmt.reset(
                        con->prepareStatement(
                            "INSERT INTO curriculum_subjects "
                            "(curriculum_id, subject_id, uneb_subject_code, subject_group, status) "
                            "VALUES (?, ?, ?, ?, 'Active')"
                        )
                    );

                    stmt->setInt(1, curriculum->Id);
                    stmt->setInt(2, subject->Id);
                    stmt->setString(3, code);
                    stmt->setString(4, group);
                    stmt->executeUpdate();
                }

                editorForm->Close();
                LoadSubjectRecords(txtSearch == nullptr ? L"" : txtSearch->Text);
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(gcnew String(ex.what()), L"Database Error",
                    MessageBoxButtons::OK, MessageBoxIcon::Error);
            }
        }

        void OpenSubjectEditor(int recordId)
        {
            editSubjectMode = recordId > 0;
            editingCurriculumSubjectId = recordId;

            editorForm = gcnew Form();
            editorForm->Text = editSubjectMode
                ? L"Edit UNEB Subject"
                : L"Register UNEB Subject";
            editorForm->StartPosition = FormStartPosition::CenterParent;
            editorForm->FormBorderStyle = System::Windows::Forms::FormBorderStyle::Sizable;
            editorForm->MaximizeBox = false;
            editorForm->MinimizeBox = false;
            editorForm->ShowInTaskbar = false;
            editorForm->MinimumSize = System::Drawing::Size(680, 430);
            editorForm->ClientSize = System::Drawing::Size(760, 500);
            editorForm->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;

            TableLayoutPanel^ layout = gcnew TableLayoutPanel();
            layout->Dock = DockStyle::Fill;
            layout->Padding = System::Windows::Forms::Padding(24);
            layout->ColumnCount = 2;
            layout->RowCount = 5;
            layout->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Absolute, 190.0F));
            layout->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Percent, 100.0F));

            for (int i = 0; i < 5; ++i)
                layout->RowStyles->Add(
                    gcnew RowStyle(SizeType::Absolute, i == 4 ? 58.0F : 52.0F));

            cmbCurriculum = gcnew ComboBox();
            cmbSubject = gcnew ComboBox();
            cmbGroup = gcnew ComboBox();
            txtUnebSubjectCode = gcnew TextBox();

            cmbCurriculum->DropDownStyle = ComboBoxStyle::DropDownList;
            cmbSubject->DropDownStyle = ComboBoxStyle::DropDownList;
            cmbGroup->DropDownStyle = ComboBoxStyle::DropDownList;

            cmbGroup->Items->Add(L"Humanities");
            cmbGroup->Items->Add(L"Sciences");
            cmbGroup->Items->Add(L"Languages");
            cmbGroup->Items->Add(L"Business");
            cmbGroup->Items->Add(L"Technical / Vocational");
            cmbGroup->Items->Add(L"General / Other");
            cmbGroup->SelectedIndex = 0;

            AddField(layout, L"Curriculum", cmbCurriculum, 0);
            AddField(layout, L"Subject", cmbSubject, 1);
            AddField(layout, L"UNEB Subject Code", txtUnebSubjectCode, 2);
            AddField(layout, L"Subject Group", cmbGroup, 3);

            Label^ note = gcnew Label();
            note->Text =
                L"Enter the official UNEB subject code for this curriculum-specific subject record. "
                L"Paper codes are configured separately.";
            note->Dock = DockStyle::Fill;
            note->ForeColor = Color::DimGray;
            note->TextAlign = ContentAlignment::MiddleLeft;
            layout->Controls->Add(note, 0, 4);
            layout->SetColumnSpan(note, 2);

            FlowLayoutPanel^ buttons = gcnew FlowLayoutPanel();
            buttons->Dock = DockStyle::Bottom;
            buttons->Height = 54;
            buttons->FlowDirection = FlowDirection::RightToLeft;
            buttons->WrapContents = false;
            buttons->Padding = System::Windows::Forms::Padding(0, 8, 0, 8);

            Button^ save = gcnew Button();
            save->Text = editSubjectMode ? L"Save Changes" : L"Register";
            save->Size = System::Drawing::Size(140, 38);

            Button^ cancel = gcnew Button();
            cancel->Text = L"Cancel";
            cancel->Size = System::Drawing::Size(100, 38);

            buttons->Controls->Add(save);
            buttons->Controls->Add(cancel);

            cancel->Click += gcnew EventHandler(this,
                &UnebSubjectPaperManagement::EditorCancelClicked);
            save->Click += gcnew EventHandler(this,
                &UnebSubjectPaperManagement::EditorSaveClicked);

            editorForm->Controls->Add(layout);
            editorForm->Controls->Add(buttons);

            LoadCurricula(0);
            LoadSubjects(0);

            if (editSubjectMode)
                LoadSubjectEditorForEdit(recordId);

            ThemeManager::ApplyToForm(editorForm);
            editorForm->ShowDialog(this);
        }

        void LoadPapers(String^ searchText)
        {
            papersGrid->Rows->Clear();
            searchText = String::IsNullOrWhiteSpace(searchText) ? L"" : searchText->Trim();

            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT "
                        "paper_id, paper_code, paper_number, paper_name, paper_type, status "
                        "FROM subject_papers "
                        "WHERE curriculum_subject_id = ? AND (? = '' OR paper_code LIKE ? "
                        "OR paper_number LIKE ? OR paper_name LIKE ? OR paper_type LIKE ? OR status LIKE ?) "
                        "ORDER BY paper_code"
                    )
                );

                stmt->setInt(1, selectedCurriculumSubjectId);
                std::string paperSearch = msclr::interop::marshal_as<std::string>(searchText);
                std::string paperPattern = msclr::interop::marshal_as<std::string>(L"%" + searchText + L"%");
                stmt->setString(2, paperSearch);
                stmt->setString(3, paperPattern);
                stmt->setString(4, paperPattern);
                stmt->setString(5, paperPattern);
                stmt->setString(6, paperPattern);
                stmt->setString(7, paperPattern);

                std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());

                while (result->next())
                {
                    papersGrid->Rows->Add(
                        result->getInt("paper_id"),
                        gcnew String(result->getString("paper_code").c_str()),
                        result->isNull("paper_number")
                            ? L""
                            : gcnew String(result->getString("paper_number").c_str()),
                        result->isNull("paper_name")
                            ? L""
                            : gcnew String(result->getString("paper_name").c_str()),
                        gcnew String(result->getString("paper_type").c_str()),
                        gcnew String(result->getString("status").c_str())
                    );
                }
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(gcnew String(ex.what()), L"Database Error",
                    MessageBoxButtons::OK, MessageBoxIcon::Error);
            }
        }

        void OpenPapers(int recordId)
        {
            if (recordId == 0)
                return;

            selectedCurriculumSubjectId = recordId;

            papersForm = gcnew Form();
            papersForm->Text = L"UNEB Papers";
            papersForm->StartPosition = FormStartPosition::CenterParent;
            papersForm->FormBorderStyle = System::Windows::Forms::FormBorderStyle::Sizable;
            papersForm->MaximizeBox = true;
            papersForm->MinimizeBox = true;
            papersForm->ShowInTaskbar = false;
            papersForm->MinimumSize = System::Drawing::Size(880, 560);
            papersForm->ClientSize = System::Drawing::Size(1000, 650);

            TableLayoutPanel^ layout = gcnew TableLayoutPanel();
            layout->Dock = DockStyle::Fill;
            layout->Padding = System::Windows::Forms::Padding(20);
            layout->ColumnCount = 1;
            layout->RowCount = 4;
            layout->RowStyles->Add(gcnew RowStyle(SizeType::Absolute, 62.0F));
            layout->RowStyles->Add(gcnew RowStyle(SizeType::Absolute, 48.0F));
            layout->RowStyles->Add(gcnew RowStyle(SizeType::Percent, 100.0F));
            layout->RowStyles->Add(gcnew RowStyle(SizeType::Absolute, 52.0F));

            Label^ heading = gcnew Label();
            heading->Text = L"Papers for Selected UNEB Subject";
            heading->Dock = DockStyle::Fill;
            heading->Font = gcnew System::Drawing::Font(L"Segoe UI Semibold", 15.0F, FontStyle::Bold);
            heading->ForeColor = Color::FromArgb(30, 41, 59);
            heading->TextAlign = ContentAlignment::MiddleLeft;
            layout->Controls->Add(heading, 0, 0);

            FlowLayoutPanel^ paperSearchPanel = gcnew FlowLayoutPanel();
            paperSearchPanel->Dock = DockStyle::Fill;
            paperSearchPanel->WrapContents = false;
            paperSearchPanel->FlowDirection = FlowDirection::LeftToRight;
            paperSearchPanel->Padding = System::Windows::Forms::Padding(0, 5, 0, 5);
            txtPaperSearch = gcnew TextBox();
            txtPaperSearch->Width = 340;
            txtPaperSearch->Height = 32;
            txtPaperSearch->MaxLength = 150;
            btnPaperSearch = gcnew Button();
            btnPaperSearch->Text = L"Search";
            btnPaperSearch->Size = System::Drawing::Size(90, 32);
            btnPaperSearch->Margin = System::Windows::Forms::Padding(6, 0, 0, 0);
            paperSearchPanel->Controls->Add(txtPaperSearch);
            paperSearchPanel->Controls->Add(btnPaperSearch);
            layout->Controls->Add(paperSearchPanel, 0, 1);

            papersGrid = gcnew DataGridView();
            papersGrid->Dock = DockStyle::Fill;
            papersGrid->ReadOnly = true;
            papersGrid->AllowUserToAddRows = false;
            papersGrid->AllowUserToDeleteRows = false;
            papersGrid->RowHeadersVisible = false;
            papersGrid->SelectionMode = DataGridViewSelectionMode::FullRowSelect;
            papersGrid->MultiSelect = false;
            papersGrid->AutoGenerateColumns = false;
            papersGrid->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::Fill;

            papersGrid->Columns->Add(L"PaperId", L"ID");
            papersGrid->Columns["PaperId"]->Visible = false;
            papersGrid->Columns->Add(L"PaperCode", L"Paper Code");
            papersGrid->Columns->Add(L"PaperNumber", L"Paper No.");
            papersGrid->Columns->Add(L"PaperName", L"Paper");
            papersGrid->Columns->Add(L"PaperType", L"Type");
            papersGrid->Columns->Add(L"Status", L"Status");

            layout->Controls->Add(papersGrid, 0, 2);

            FlowLayoutPanel^ buttons = gcnew FlowLayoutPanel();
            buttons->Dock = DockStyle::Fill;
            buttons->FlowDirection = FlowDirection::RightToLeft;
            buttons->WrapContents = false;
            buttons->Padding = System::Windows::Forms::Padding(0, 7, 0, 0);

            Button^ close = gcnew Button();
            close->Text = L"Close";
            close->Size = System::Drawing::Size(100, 38);

            Button^ toggle = gcnew Button();
            toggle->Text = L"Activate / Deactivate";
            toggle->Size = System::Drawing::Size(165, 38);

            Button^ edit = gcnew Button();
            edit->Text = L"Edit Paper";
            edit->Size = System::Drawing::Size(110, 38);

            Button^ add = gcnew Button();
            add->Text = L"New Paper";
            add->Size = System::Drawing::Size(110, 38);

            buttons->Controls->Add(close);
            buttons->Controls->Add(toggle);
            buttons->Controls->Add(edit);
            buttons->Controls->Add(add);
            layout->Controls->Add(buttons, 0, 3);

            add->Click += gcnew EventHandler(this,
                &UnebSubjectPaperManagement::NewPaperClicked);
            edit->Click += gcnew EventHandler(this,
                &UnebSubjectPaperManagement::EditPaperClicked);
            toggle->Click += gcnew EventHandler(this,
                &UnebSubjectPaperManagement::TogglePaperClicked);            close->Click += gcnew EventHandler(this,
                &UnebSubjectPaperManagement::ClosePapersClicked);

            papersForm->Controls->Add(layout);
            btnPaperSearch->Click += gcnew EventHandler(this, &UnebSubjectPaperManagement::PaperSearchClicked);
            txtPaperSearch->KeyDown += gcnew KeyEventHandler(this, &UnebSubjectPaperManagement::PaperSearchKeyDown);
            LoadPapers(L"");
            ThemeManager::ApplyToForm(papersForm);
            papersForm->ShowDialog(this);
        }

        int GetSelectedPaperId()
        {
            if (papersGrid == nullptr || papersGrid->SelectedRows->Count == 0)
                return 0;

            Object^ value = papersGrid->SelectedRows[0]->Cells["PaperId"]->Value;
            return value == nullptr ? 0 : Convert::ToInt32(value);
        }

        void LoadPaperEditorForEdit(int paperId)
        {
            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT paper_code, paper_number, paper_name, paper_type "
                        "FROM subject_papers "
                        "WHERE paper_id = ?"
                    )
                );

                stmt->setInt(1, paperId);

                std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());

                if (!result->next())
                    return;

                txtPaperCode->Text =
                    gcnew String(result->getString("paper_code").c_str());

                if (!result->isNull("paper_number"))
                    txtPaperNumber->Text =
                        gcnew String(result->getString("paper_number").c_str());

                if (!result->isNull("paper_name"))
                    txtPaperName->Text =
                        gcnew String(result->getString("paper_name").c_str());

                String^ type =
                    gcnew String(result->getString("paper_type").c_str());

                for (int i = 0; i < cmbPaperType->Items->Count; ++i)
                {
                    if (String::Equals(
                            cmbPaperType->Items[i]->ToString(),
                            type,
                            StringComparison::OrdinalIgnoreCase))
                    {
                        cmbPaperType->SelectedIndex = i;
                        break;
                    }
                }
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(gcnew String(ex.what()), L"Database Error",
                    MessageBoxButtons::OK, MessageBoxIcon::Error);
            }
        }

        void OpenPaperEditor(int paperId)
        {
            editPaperMode = paperId > 0;
            editingPaperId = paperId;

            paperEditorForm = gcnew Form();
            paperEditorForm->Text = editPaperMode ? L"Edit UNEB Paper" : L"Register UNEB Paper";
            paperEditorForm->StartPosition = FormStartPosition::CenterParent;
            paperEditorForm->FormBorderStyle = System::Windows::Forms::FormBorderStyle::Sizable;
            paperEditorForm->MaximizeBox = false;
            paperEditorForm->MinimizeBox = false;
            paperEditorForm->ShowInTaskbar = false;
            paperEditorForm->MinimumSize = System::Drawing::Size(650, 420);
            paperEditorForm->ClientSize = System::Drawing::Size(720, 500);

            TableLayoutPanel^ layout = gcnew TableLayoutPanel();
            layout->Dock = DockStyle::Fill;
            layout->Padding = System::Windows::Forms::Padding(24);
            layout->ColumnCount = 2;
            layout->RowCount = 5;
            layout->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Absolute, 160.0F));
            layout->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Percent, 100.0F));

            for (int i = 0; i < 5; ++i)
                layout->RowStyles->Add(
                    gcnew RowStyle(SizeType::Absolute, i == 4 ? 58.0F : 52.0F));

            txtPaperCode = gcnew TextBox();
            txtPaperNumber = gcnew TextBox();
            txtPaperName = gcnew TextBox();
            cmbPaperType = gcnew ComboBox();

            cmbPaperType->DropDownStyle = ComboBoxStyle::DropDownList;
            cmbPaperType->Items->Add(L"Theory");
            cmbPaperType->Items->Add(L"Practical");
            cmbPaperType->Items->Add(L"Oral");
            cmbPaperType->Items->Add(L"Project");
            cmbPaperType->Items->Add(L"Alternative Practical");
            cmbPaperType->Items->Add(L"Other");
            cmbPaperType->SelectedIndex = 0;

            AddField(layout, L"Paper Code", txtPaperCode, 0);
            AddField(layout, L"Paper Number", txtPaperNumber, 1);
            AddField(layout, L"Paper Name", txtPaperName, 2);
            AddField(layout, L"Paper Type", cmbPaperType, 3);

            Label^ note = gcnew Label();
            note->Text =
                L"Paper Code is entered exactly as the official UNEB code. "
                L"It is not generated from the subject.";
            note->Dock = DockStyle::Fill;
            note->ForeColor = Color::DimGray;
            note->TextAlign = ContentAlignment::MiddleLeft;
            layout->Controls->Add(note, 0, 4);
            layout->SetColumnSpan(note, 2);

            FlowLayoutPanel^ buttons = gcnew FlowLayoutPanel();
            buttons->Dock = DockStyle::Bottom;
            buttons->Height = 54;
            buttons->FlowDirection = FlowDirection::RightToLeft;
            buttons->WrapContents = false;
            buttons->Padding = System::Windows::Forms::Padding(0, 8, 0, 8);

            Button^ save = gcnew Button();
            save->Text = editPaperMode ? L"Save Changes" : L"Register Paper";
            save->Size = System::Drawing::Size(140, 38);

            Button^ cancel = gcnew Button();
            cancel->Text = L"Cancel";
            cancel->Size = System::Drawing::Size(100, 38);

            buttons->Controls->Add(save);
            buttons->Controls->Add(cancel);

            cancel->Click += gcnew EventHandler(this,
                &UnebSubjectPaperManagement::PaperEditorCancelClicked);
            save->Click += gcnew EventHandler(this,
                &UnebSubjectPaperManagement::PaperEditorSaveClicked);

            paperEditorForm->Controls->Add(layout);
            paperEditorForm->Controls->Add(buttons);

            if (editPaperMode)
                LoadPaperEditorForEdit(paperId);

            ThemeManager::ApplyToForm(paperEditorForm);
            paperEditorForm->ShowDialog(this);
        }

        void SavePaper()
        {
            if (String::IsNullOrWhiteSpace(txtPaperCode->Text))
            {
                MessageBox::Show(
                    L"Enter the official UNEB paper code.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                txtPaperCode->Focus();
                return;
            }

            if (cmbPaperType->SelectedIndex < 0)
            {
                MessageBox::Show(
                    L"Select the paper type.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                return;
            }

            try
            {
                auto con = DbConnection::GetConnection();

                int subjectId = 0;
                int levelId = 0;

                std::unique_ptr<sql::PreparedStatement> info(
                    con->prepareStatement(
                        "SELECT "
                        "cs.subject_id, "
                        "c.academic_level_id "
                        "FROM curriculum_subjects cs "
                        "INNER JOIN curricula c "
                        "ON c.curriculum_id = cs.curriculum_id "
                        "WHERE cs.curriculum_subject_id = ?"
                    )
                );

                info->setInt(1, selectedCurriculumSubjectId);

                std::unique_ptr<sql::ResultSet> infoResult(info->executeQuery());

                if (!infoResult->next())
                {
                    MessageBox::Show(
                        L"The selected UNEB subject could not be found.",
                        L"Validation",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Warning
                    );
                    return;
                }

                subjectId = infoResult->getInt("subject_id");
                levelId = infoResult->getInt("academic_level_id");

                std::string code =
                    msclr::interop::marshal_as<std::string>(
                        txtPaperCode->Text->Trim()->ToUpperInvariant());

                std::string number =
                    msclr::interop::marshal_as<std::string>(
                        txtPaperNumber->Text->Trim());

                std::string name =
                    msclr::interop::marshal_as<std::string>(
                        txtPaperName->Text->Trim());

                std::string type =
                    msclr::interop::marshal_as<std::string>(
                        cmbPaperType->SelectedItem->ToString());

                std::unique_ptr<sql::PreparedStatement> stmt;

                if (editPaperMode)
                {
                    stmt.reset(
                        con->prepareStatement(
                            "UPDATE subject_papers "
                            "SET curriculum_subject_id = ?, "
                            "subject_id = ?, "
                            "academic_level_id = ?, "
                            "paper_code = ?, "
                            "paper_number = ?, "
                            "paper_name = ?, "
                            "paper_type = ? "
                            "WHERE paper_id = ?"
                        )
                    );

                    stmt->setInt(1, selectedCurriculumSubjectId);
                    stmt->setInt(2, subjectId);
                    stmt->setInt(3, levelId);
                    stmt->setString(4, code);
                    stmt->setString(5, number);
                    stmt->setString(6, name);
                    stmt->setString(7, type);
                    stmt->setInt(8, editingPaperId);
                }
                else
                {
                    stmt.reset(
                        con->prepareStatement(
                            "INSERT INTO subject_papers "
                            "(curriculum_subject_id, subject_id, academic_level_id, "
                            "paper_code, paper_number, paper_name, paper_type, status) "
                            "VALUES (?, ?, ?, ?, ?, ?, ?, 'Active')"
                        )
                    );

                    stmt->setInt(1, selectedCurriculumSubjectId);
                    stmt->setInt(2, subjectId);
                    stmt->setInt(3, levelId);
                    stmt->setString(4, code);
                    stmt->setString(5, number);
                    stmt->setString(6, name);
                    stmt->setString(7, type);
                }

                stmt->executeUpdate();

                paperEditorForm->Close();
                LoadPapers(txtPaperSearch == nullptr ? L"" : txtPaperSearch->Text);
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(gcnew String(ex.what()), L"Database Error",
                    MessageBoxButtons::OK, MessageBoxIcon::Error);
            }
        }

        void TogglePaper()
        {
            int id = GetSelectedPaperId();
            if (id == 0)
                return;

            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "UPDATE subject_papers "
                        "SET status = CASE "
                        "WHEN status = 'Active' THEN 'Inactive' "
                        "ELSE 'Active' END "
                        "WHERE paper_id = ?"
                    )
                );

                stmt->setInt(1, id);
                stmt->executeUpdate();
                LoadPapers(
                    txtPaperSearch == nullptr ? L"" : txtPaperSearch->Text
                );
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(gcnew String(ex.what()), L"Database Error",
                    MessageBoxButtons::OK, MessageBoxIcon::Error);
            }
        }

        void ToggleSubject()
        {
            int id = GetSelectedSubjectRecordId();
            if (id == 0)
                return;

            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "UPDATE curriculum_subjects "
                        "SET status = CASE "
                        "WHEN status = 'Active' THEN 'Inactive' "
                        "ELSE 'Active' END "
                        "WHERE curriculum_subject_id = ?"
                    )
                );

                stmt->setInt(1, id);
                stmt->executeUpdate();
                LoadSubjectRecords(txtSearch == nullptr ? L"" : txtSearch->Text);            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(gcnew String(ex.what()), L"Database Error",
                    MessageBoxButtons::OK, MessageBoxIcon::Error);
            }
        }

        System::Void EditorCancelClicked(Object^ sender, EventArgs^ e)
        {
            if (editorForm != nullptr)
                editorForm->Close();
        }

        System::Void EditorSaveClicked(Object^ sender, EventArgs^ e)
        {
            SaveSubjectRecord();
        }

        System::Void PaperEditorCancelClicked(Object^ sender, EventArgs^ e)
        {
            if (paperEditorForm != nullptr)
                paperEditorForm->Close();
        }

        System::Void PaperEditorSaveClicked(Object^ sender, EventArgs^ e)
        {
            SavePaper();
        }

        System::Void NewSubjectClicked(Object^ sender, EventArgs^ e)
        {
            OpenSubjectEditor(0);
        }

        System::Void EditSubjectClicked(Object^ sender, EventArgs^ e)
        {
            OpenSubjectEditor(GetSelectedSubjectRecordId());
        }

        System::Void ManagePapersClicked(Object^ sender, EventArgs^ e)
        {
            OpenPapers(GetSelectedSubjectRecordId());
        }

        System::Void ToggleSubjectClicked(Object^ sender, EventArgs^ e)
        {
            ToggleSubject();
        }

        System::Void PaperSearchClicked(Object^ sender, EventArgs^ e)
        {
            LoadPapers(txtPaperSearch == nullptr ? L"" : txtPaperSearch->Text);
        }

        System::Void PaperSearchKeyDown(Object^ sender, KeyEventArgs^ e)
        {
            if (e->KeyCode == Keys::Enter)
            {
                PaperSearchClicked(sender, e);
                e->SuppressKeyPress = true;
            }
        }

        System::Void NewPaperClicked(Object^ sender, EventArgs^ e)
        {
            OpenPaperEditor(0);
        }

        System::Void EditPaperClicked(Object^ sender, EventArgs^ e)
        {
            OpenPaperEditor(GetSelectedPaperId());
        }

        System::Void TogglePaperClicked(Object^ sender, EventArgs^ e)
        {
            TogglePaper();
        }

        System::Void ClosePapersClicked(Object^ sender, EventArgs^ e)
        {
            if (papersForm != nullptr)
                papersForm->Close();
        }

        System::Void GridSelectionChanged(Object^ sender, EventArgs^ e)
        {
            UpdateSubjectActions();
        }

        System::Void BackClicked(Object^ sender, EventArgs^ e)
        {
            Close();
        }

    public:
        UnebSubjectPaperManagement(void)
        {
            InitializeComponent();
            ThemeManager::ApplyToForm(this);

            if (LicenseManager::UsageMode != LicenseUsageMode::Designtime)
                LoadSubjectRecords(
                    txtSearch == nullptr ? L"" : txtSearch->Text
                );
        }

    protected:
        ~UnebSubjectPaperManagement()
        {
            if (components)
                delete components;
        }

#pragma region Windows Form Designer generated code

        void InitializeComponent(void)
        {
            components = gcnew System::ComponentModel::Container();

            mainLayout = gcnew TableLayoutPanel();
            headerPanel = gcnew Panel();
            titleLabel = gcnew Label();
            subtitleLabel = gcnew Label();
            actionPanel = gcnew FlowLayoutPanel();
            btnNewSubject = gcnew Button();
            btnEditSubject = gcnew Button();
            btnManagePapers = gcnew Button();
            btnToggleSubject = gcnew Button();
            subjectGrid = gcnew DataGridView();
            bottomPanel = gcnew FlowLayoutPanel();
            btnBack = gcnew Button();

            SuspendLayout();

            Text = L"UNEB Subjects & Papers";
            StartPosition = FormStartPosition::CenterScreen;
            WindowState = FormWindowState::Maximized;
            MinimumSize = System::Drawing::Size(1100, 680);
            AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            BackColor = Color::FromArgb(248, 250, 252);

            mainLayout->Dock = DockStyle::Fill;
            mainLayout->ColumnCount = 1;
            mainLayout->RowCount = 4;
            mainLayout->Padding = System::Windows::Forms::Padding(28, 20, 28, 14);
            mainLayout->RowStyles->Add(gcnew RowStyle(SizeType::Absolute, 78.0F));
            mainLayout->RowStyles->Add(gcnew RowStyle(SizeType::Absolute, 58.0F));
            mainLayout->RowStyles->Add(gcnew RowStyle(SizeType::Percent, 100.0F));
            mainLayout->RowStyles->Add(gcnew RowStyle(SizeType::Absolute, 50.0F));

            headerPanel->Dock = DockStyle::Fill;
            titleLabel->Text = L"UNEB Subjects & Papers";
            titleLabel->Dock = DockStyle::Top;
            titleLabel->Height = 38;
            titleLabel->Font = gcnew System::Drawing::Font(L"Segoe UI Semibold", 21.0F, FontStyle::Bold);
            titleLabel->ForeColor = Color::FromArgb(30, 41, 59);

            subtitleLabel->Text =
                L"Maintain curriculum-specific UNEB subject codes and enter each official paper code separately.";
            subtitleLabel->Dock = DockStyle::Fill;
            subtitleLabel->Font = gcnew System::Drawing::Font(L"Segoe UI", 9.5F);
            subtitleLabel->ForeColor = Color::DimGray;

            headerPanel->Controls->Add(subtitleLabel);
            headerPanel->Controls->Add(titleLabel);

            actionPanel->Dock = DockStyle::Fill;
            actionPanel->FlowDirection = FlowDirection::LeftToRight;
            actionPanel->WrapContents = false;
            actionPanel->AutoScroll = true;
            actionPanel->Padding = System::Windows::Forms::Padding(0, 8, 0, 0);

            ConfigureButton(btnNewSubject, L"New UNEB Subject", 150);
            ConfigureButton(btnEditSubject, L"Edit", 90);
            ConfigureButton(btnManagePapers, L"Manage Papers", 140);
            ConfigureButton(btnToggleSubject, L"Activate / Deactivate", 165);

            lblSearch = gcnew Label();
            lblSearch->Text = L"Search";
            lblSearch->Width = 55;
            lblSearch->Height = 36;
            lblSearch->TextAlign = ContentAlignment::MiddleLeft;
            lblSearch->Margin = System::Windows::Forms::Padding(18, 1, 4, 0);
            txtSearch = gcnew TextBox();
            txtSearch->Width = 300;
            txtSearch->Height = 36;
            txtSearch->MaxLength = 150;
            btnSearch = gcnew Button();
            btnSearch->Text = L"Search";
            btnSearch->Width = 90;
            btnSearch->Height = 36;
            btnSearch->FlatStyle = FlatStyle::Flat;
            btnSearch->FlatAppearance->BorderSize = 0;

            btnEditSubject->Enabled = false;
            btnManagePapers->Enabled = false;
            btnToggleSubject->Enabled = false;

            actionPanel->Controls->Add(btnNewSubject);
            actionPanel->Controls->Add(btnEditSubject);
            actionPanel->Controls->Add(btnManagePapers);
            actionPanel->Controls->Add(btnToggleSubject);
            actionPanel->Controls->Add(lblSearch);
            actionPanel->Controls->Add(txtSearch);
            actionPanel->Controls->Add(btnSearch);
            btnSearch->Click += gcnew EventHandler(this, &UnebSubjectPaperManagement::SubjectSearchClicked);
            txtSearch->KeyDown += gcnew KeyEventHandler(this, &UnebSubjectPaperManagement::SubjectSearchKeyDown);

            subjectGrid->Dock = DockStyle::Fill;
            subjectGrid->ReadOnly = true;
            subjectGrid->AllowUserToAddRows = false;
            subjectGrid->AllowUserToDeleteRows = false;
            subjectGrid->RowHeadersVisible = false;
            subjectGrid->MultiSelect = false;
            subjectGrid->SelectionMode = DataGridViewSelectionMode::FullRowSelect;
            subjectGrid->AutoGenerateColumns = false;
            subjectGrid->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::Fill;
            subjectGrid->RowTemplate->Height = 38;
            subjectGrid->BackgroundColor = Color::White;
            subjectGrid->BorderStyle = BorderStyle::None;
            subjectGrid->ColumnHeadersHeight = 40;

            subjectGrid->Columns->Add(L"CurriculumSubjectId", L"ID");
            subjectGrid->Columns["CurriculumSubjectId"]->Visible = false;
            subjectGrid->Columns->Add(L"Curriculum", L"Curriculum");
            subjectGrid->Columns->Add(L"Level", L"Level");
            subjectGrid->Columns->Add(L"Group", L"Group");
            subjectGrid->Columns->Add(L"Subject", L"Subject");
            subjectGrid->Columns->Add(L"UnebSubjectCode", L"UNEB Subject Code");
            subjectGrid->Columns->Add(L"Papers", L"Papers");
            subjectGrid->Columns->Add(L"Status", L"Status");

            bottomPanel->Dock = DockStyle::Fill;
            bottomPanel->FlowDirection = FlowDirection::LeftToRight;
            bottomPanel->WrapContents = false;
            bottomPanel->Padding = System::Windows::Forms::Padding(0, 5, 0, 0);

            ConfigureButton(btnBack, L"Back to Dashboard", 150);
            bottomPanel->Controls->Add(btnBack);

            mainLayout->Controls->Add(headerPanel, 0, 0);
            mainLayout->Controls->Add(actionPanel, 0, 1);
            mainLayout->Controls->Add(subjectGrid, 0, 2);
            mainLayout->Controls->Add(bottomPanel, 0, 3);

            Controls->Add(mainLayout);

                        btnNewSubject->Click += gcnew EventHandler(this, &UnebSubjectPaperManagement::NewSubjectClicked);
            btnEditSubject->Click += gcnew EventHandler(this, &UnebSubjectPaperManagement::EditSubjectClicked);
            btnManagePapers->Click += gcnew EventHandler(this, &UnebSubjectPaperManagement::ManagePapersClicked);
            btnToggleSubject->Click += gcnew EventHandler(this, &UnebSubjectPaperManagement::ToggleSubjectClicked);
            btnBack->Click += gcnew EventHandler(this, &UnebSubjectPaperManagement::BackClicked);
            subjectGrid->SelectionChanged += gcnew EventHandler(this, &UnebSubjectPaperManagement::GridSelectionChanged);

            ResumeLayout(false);
        }

#pragma endregion
    };
}
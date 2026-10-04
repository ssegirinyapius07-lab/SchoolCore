#pragma once

#include "DbConnection.h"
#include "ThemeManager.h"
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
    public ref class MarksEntryManagement : public Form
    {
    private:

        ref class Item
        {
        public:
            int Id;
            String^ Text;

            Item(int id, String^ text)
            {
                Id = id;
                Text = text;
            }

            virtual String^ ToString() override
            {
                return Text;
            }
        };

        int examinationId = 0;

        TableLayoutPanel^ root;
        Panel^ headerPanel;
        Label^ lblTitle;
        Label^ lblSubtitle;

        Panel^ filterPanel;
        ComboBox^ cmbSubject;
        ComboBox^ cmbPaper;
        Label^ lblMaxScore;
        Label^ lblPassMark;
        Label^ lblStudentCount;
        Button^ btnRefresh;
        Button^ btnSave;
        Button^ btnClose;

        DataGridView^ marksGrid;

        bool loading = false;

        int GetSelectedId(ComboBox^ combo)
        {
            if (combo == nullptr || combo->SelectedItem == nullptr)
                return 0;

            Item^ item = dynamic_cast<Item^>(combo->SelectedItem);

            return item == nullptr ? 0 : item->Id;
        }

        String^ GetString(
            sql::ResultSet* result,
            const char* column)
        {
            if (result->isNull(column))
                return L"";

            return gcnew String(
                result->getString(column).c_str()
            );
        }

        void ShowDatabaseError(sql::SQLException& ex)
        {
            MessageBox::Show(
                gcnew String(ex.what()),
                L"Marks Entry",
                MessageBoxButtons::OK,
                MessageBoxIcon::Error
            );
        }

        void LoadSubjects()
        {
            this->cmbSubject->Items->Clear();
            this->cmbPaper->Items->Clear();

            this->cmbPaper->Enabled = false;
            this->lblMaxScore->Text = L"Maximum Score: —";
            this->lblPassMark->Text = L"Pass Mark: —";
            this->lblStudentCount->Text = L"Students: 0";

            if (this->examinationId == 0)
                return;

            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT DISTINCT "
                        "s.subject_id, "
                        "s.subject_name "
                        "FROM examination_papers ep "
                        "INNER JOIN examination_subjects es "
                        "ON es.examination_subject_id = ep.examination_subject_id "
                        "INNER JOIN subjects s "
                        "ON s.subject_id = es.subject_id "
                        "WHERE es.examination_id = ? "
                        "AND ep.status = 'Active' "
                        "ORDER BY s.subject_name ASC"
                    )
                );

                stmt->setInt(1, this->examinationId);

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                while (result->next())
                {
                    this->cmbSubject->Items->Add(
                        gcnew Item(
                            result->getInt("subject_id"),
                            GetString(result.get(), "subject_name")
                        )
                    );
                }

                if (this->cmbSubject->Items->Count > 0)
                    this->cmbSubject->SelectedIndex = 0;
                else
                {
                    this->marksGrid->Rows->Clear();
                    MessageBox::Show(
                        L"No examination papers have been assigned to this examination yet.",
                        L"Marks Entry",
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

        void LoadPapers()
        {
            this->cmbPaper->Items->Clear();
            this->cmbPaper->Enabled = false;

            this->lblMaxScore->Text = L"Maximum Score: —";
            this->lblPassMark->Text = L"Pass Mark: —";

            int subjectId = GetSelectedId(this->cmbSubject);

            if (subjectId == 0)
            {
                this->marksGrid->Rows->Clear();
                this->lblStudentCount->Text = L"Students: 0";
                return;
            }

            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT "
                        "ep.examination_paper_id, "
                        "sp.paper_code, "
                        "sp.paper_name, "
                        "ep.max_score, "
                        "ep.pass_mark "
                        "FROM examination_papers ep "
                        "INNER JOIN examination_subjects es "
                        "ON es.examination_subject_id = ep.examination_subject_id "
                        "INNER JOIN subject_papers sp "
                        "ON sp.paper_id = ep.paper_id "
                        "WHERE es.examination_id = ? "
                        "AND es.subject_id = ? "
                        "AND ep.status = 'Active' "
                        "ORDER BY sp.paper_code ASC"
                    )
                );

                stmt->setInt(1, this->examinationId);
                stmt->setInt(2, subjectId);

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                while (result->next())
                {
                    String^ code =
                        GetString(result.get(), "paper_code");

                    String^ name =
                        GetString(result.get(), "paper_name");

                    String^ display =
                        String::IsNullOrWhiteSpace(name)
                        ? code
                        : code + L" - " + name;

                    this->cmbPaper->Items->Add(
                        gcnew Item(
                            result->getInt("examination_paper_id"),
                            display
                        )
                    );
                }

                this->cmbPaper->Enabled =
                    this->cmbPaper->Items->Count > 0;

                if (this->cmbPaper->Items->Count > 0)
                    this->cmbPaper->SelectedIndex = 0;
                else
                {
                    this->marksGrid->Rows->Clear();
                    this->lblStudentCount->Text = L"Students: 0";
                }
            }
            catch (sql::SQLException& ex)
            {
                ShowDatabaseError(ex);
            }
        }

        void LoadStudents()
        {
            this->marksGrid->Rows->Clear();
            this->lblStudentCount->Text = L"Students: 0";

            int paperId = GetSelectedId(this->cmbPaper);

            if (paperId == 0)
                return;

            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> examStmt(
                    con->prepareStatement(
                        "SELECT "
                        "academic_year_id, "
                        "term_id, "
                        "class_id "
                        "FROM examinations "
                        "WHERE examination_id = ?"
                    )
                );

                examStmt->setInt(1, this->examinationId);

                std::unique_ptr<sql::ResultSet> examResult(
                    examStmt->executeQuery()
                );

                if (!examResult->next())
                    return;

                int academicYearId =
                    examResult->getInt("academic_year_id");

                int termId =
                    examResult->getInt("term_id");

                int classId =
                    examResult->getInt("class_id");

                std::unique_ptr<sql::PreparedStatement> paperStmt(
                    con->prepareStatement(
                        "SELECT "
                        "ep.max_score, "
                        "ep.pass_mark, "
                        "s.subject_name, "
                        "sp.paper_code, "
                        "sp.paper_name "
                        "FROM examination_papers ep "
                        "INNER JOIN examination_subjects es "
                        "ON es.examination_subject_id = ep.examination_subject_id "
                        "INNER JOIN subjects s "
                        "ON s.subject_id = es.subject_id "
                        "INNER JOIN subject_papers sp "
                        "ON sp.paper_id = ep.paper_id "
                        "WHERE ep.examination_paper_id = ?"
                    )
                );

                paperStmt->setInt(1, paperId);

                std::unique_ptr<sql::ResultSet> paperResult(
                    paperStmt->executeQuery()
                );

                if (paperResult->next())
                {
                    this->lblMaxScore->Text =
                        L"Maximum Score: " +
                        paperResult->getDouble("max_score").ToString("0.##");

                    this->lblPassMark->Text =
                        paperResult->isNull("pass_mark")
                        ? L"Pass Mark: —"
                        : L"Pass Mark: " +
                          paperResult->getDouble("pass_mark").ToString("0.##");

                    String^ subjectName =
                        GetString(paperResult.get(), "subject_name");

                    String^ paperCode =
                        GetString(paperResult.get(), "paper_code");

                    String^ paperName =
                        GetString(paperResult.get(), "paper_name");

                    this->lblSubtitle->Text =
                        L"Enter marks for " +
                        subjectName +
                        L" • " +
                        paperCode +
                        (
                            String::IsNullOrWhiteSpace(paperName)
                            ? L""
                            : L" - " + paperName
                        );
                }

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT "
                        "s.student_id, "
                        "s.registration_number, "
                        "CONCAT_WS(' ', s.first_name, s.middle_name, s.last_name) AS student_name, "
                        "COALESCE(st.stream_name, '') AS stream_name, "
                        "m.score, "
                        "m.grade, "
                        "m.grade_point, "
                        "m.remarks "
                        "FROM students s "
                        "INNER JOIN enrollments e "
                        "ON e.student_id = s.student_id "
                        "AND e.academic_year_id = ? "
                        "AND e.term_id = ? "
                        "AND e.class_id = ? "
                        "LEFT JOIN streams st "
                        "ON st.stream_id = e.stream_id "
                        "LEFT JOIN marks m "
                        "ON m.student_id = s.student_id "
                        "AND m.examination_paper_id = ? "
                        "WHERE s.status = 'Active' "
                        "ORDER BY "
                        "st.stream_name ASC, "
                        "s.first_name ASC, "
                        "s.middle_name ASC, "
                        "s.last_name ASC"
                    )
                );

                stmt->setInt(1, academicYearId);
                stmt->setInt(2, termId);
                stmt->setInt(3, classId);
                stmt->setInt(4, paperId);

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                int count = 0;

                while (result->next())
                {
                    int rowIndex =
                        this->marksGrid->Rows->Add();

                    DataGridViewRow^ row =
                        this->marksGrid->Rows[rowIndex];

                    row->Cells["StudentId"]->Value =
                        result->getInt("student_id");

                    row->Cells["Registration"]->Value =
                        GetString(result.get(), "registration_number");

                    row->Cells["Student"]->Value =
                        GetString(result.get(), "student_name");

                    row->Cells["Stream"]->Value =
                        GetString(result.get(), "stream_name");

                    if (!result->isNull("score"))
                    {
                        row->Cells["Score"]->Value =
                            result->getDouble("score").ToString("0.##");
                    }

                    row->Cells["Grade"]->Value =
                        result->isNull("grade")
                        ? L""
                        : GetString(result.get(), "grade");

                    row->Cells["GradePoint"]->Value =
                        result->isNull("grade_point")
                        ? L""
                        : result->getDouble("grade_point").ToString("0.##");

                    row->Cells["Remarks"]->Value =
                        result->isNull("remarks")
                        ? L""
                        : GetString(result.get(), "remarks");

                    count++;
                }

                this->lblStudentCount->Text =
                    L"Students: " + count.ToString();
            }
            catch (sql::SQLException& ex)
            {
                ShowDatabaseError(ex);
            }
        }

        bool TryGetScore(
            Object^ value,
            double maxScore,
            double% score)
        {
            score = 0.0;

            if (value == nullptr)
                return false;

            String^ text =
                value->ToString()->Trim();

            if (String::IsNullOrWhiteSpace(text))
                return false;

            if (!Double::TryParse(text, score))
                return false;

            if (score < 0.0 || score > maxScore)
                return false;

            return true;
        }

        void SaveMarks()
        {
            int paperId = GetSelectedId(this->cmbPaper);

            if (paperId == 0)
            {
                MessageBox::Show(
                    L"Select an examination paper first.",
                    L"Marks Entry",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                return;
            }

            double maxScore = 0.0;

            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> infoStmt(
                    con->prepareStatement(
                        "SELECT max_score "
                        "FROM examination_papers "
                        "WHERE examination_paper_id = ? "
                        "AND status = 'Active'"
                    )
                );

                infoStmt->setInt(1, paperId);

                std::unique_ptr<sql::ResultSet> infoResult(
                    infoStmt->executeQuery()
                );

                if (!infoResult->next())
                    return;

                maxScore =
                    infoResult->getDouble("max_score");

                con->setAutoCommit(false);

                std::unique_ptr<sql::PreparedStatement> upsert(
                    con->prepareStatement(
                        "INSERT INTO marks "
                        "(examination_subject_id, examination_paper_id, student_id, score, grade, grade_point, remarks, entered_by) "
                        "SELECT "
                        "es.examination_subject_id, "
                        "ep.examination_paper_id, "
                        "?, "
                        "?, "
                        "NULL, "
                        "NULL, "
                        "?, "
                        "?, "
                        "FROM examination_papers ep "
                        "INNER JOIN examination_subjects es "
                        "ON es.examination_subject_id = ep.examination_subject_id "
                        "WHERE ep.examination_paper_id = ? "
                        "ON DUPLICATE KEY UPDATE "
                        "score = VALUES(score), "
                        "remarks = VALUES(remarks), "
                        "entered_by = VALUES(entered_by), "
                        "entered_at = CURRENT_TIMESTAMP"
                    )
                );

                for each (DataGridViewRow^ row in this->marksGrid->Rows)
                {
                    if (row->IsNewRow)
                        continue;

                    Object^ value =
                        row->Cells["Score"]->Value;

                    if (value == nullptr ||
                        String::IsNullOrWhiteSpace(value->ToString()))
                    {
                        continue;
                    }

                    double score = 0.0;

                    if (!TryGetScore(
                            value,
                            maxScore,
                            score))
                    {
                        con->rollback();
                        con->setAutoCommit(true);

                        MessageBox::Show(
                            L"One or more marks are invalid. Scores must be between 0 and " +
                            maxScore.ToString("0.##") +
                            L".",
                            L"Marks Entry",
                            MessageBoxButtons::OK,
                            MessageBoxIcon::Warning
                        );

                        return;
                    }

                    String^ remarks =
                        row->Cells["Remarks"]->Value == nullptr
                        ? L""
                        : row->Cells["Remarks"]->Value->ToString();

                    upsert->setInt(1, Convert::ToInt32(
                        row->Cells["StudentId"]->Value
                    ));

                    upsert->setDouble(2, score);

                    upsert->setString(
                        3,
                        msclr::interop::marshal_as<std::string>(
                            remarks
                        )
                    );

                    upsert->setInt(
                        4,
                        AuthSession::UserId
                    );

                    upsert->setInt(5, paperId);

                    upsert->executeUpdate();
                }

                con->commit();
                con->setAutoCommit(true);

                LoadStudents();

                MessageBox::Show(
                    L"Marks saved successfully for " +
                    this->cmbPaper->Text +
                    L".",
                    L"Marks Entry",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information
                );
            }
            catch (sql::SQLException& ex)
            {
                ShowDatabaseError(ex);
            }
        }

        Void SubjectChanged(
            Object^ sender,
            EventArgs^ e)
        {
            if (this->loading)
                return;

            this->loading = true;
            LoadPapers();
            this->loading = false;
        }

        Void PaperChanged(
            Object^ sender,
            EventArgs^ e)
        {
            if (this->loading)
                return;

            LoadStudents();
        }

        Void SaveClicked(
            Object^ sender,
            EventArgs^ e)
        {
            SaveMarks();
        }

        Void RefreshClicked(
            Object^ sender,
            EventArgs^ e)
        {
            LoadStudents();
        }

        Void CloseClicked(
            Object^ sender,
            EventArgs^ e)
        {
            this->Close();
        }

        void ConfigureGrid()
        {
            this->marksGrid =
                gcnew DataGridView();

            this->marksGrid->Dock =
                DockStyle::Fill;

            this->marksGrid->AllowUserToAddRows = false;
            this->marksGrid->AllowUserToDeleteRows = false;
            this->marksGrid->AllowUserToResizeRows = false;

            this->marksGrid->AutoGenerateColumns = false;
            this->marksGrid->SelectionMode =
                DataGridViewSelectionMode::FullRowSelect;
            this->marksGrid->MultiSelect = false;
            this->marksGrid->RowHeadersVisible = false;

            this->marksGrid->BackgroundColor =
                Color::White;

            this->marksGrid->BorderStyle =
                BorderStyle::None;

            this->marksGrid->ColumnHeadersHeight = 42;
            this->marksGrid->RowTemplate->Height = 34;

            this->marksGrid->Columns->Add(
                gcnew DataGridViewTextBoxColumn()
            );

            DataGridViewTextBoxColumn^ hiddenStudentId =
                safe_cast<DataGridViewTextBoxColumn^>(
                    this->marksGrid->Columns[0]
                );

            hiddenStudentId->Name = L"StudentId";
            hiddenStudentId->Visible = false;

            AddGridTextColumn(
                L"Registration",
                L"Registration",
                150
            );

            AddGridTextColumn(
                L"Student",
                L"Student",
                260
            );

            AddGridTextColumn(
                L"Stream",
                L"Stream",
                120
            );

            DataGridViewTextBoxColumn^ scoreColumn =
                AddGridTextColumn(
                    L"Score",
                    L"Score",
                    110
                );

            scoreColumn->DefaultCellStyle->Alignment =
                DataGridViewContentAlignment::MiddleCenter;

            scoreColumn->DefaultCellStyle->BackColor =
                Color::FromArgb(248, 250, 252);

            DataGridViewTextBoxColumn^ gradeColumn =
                AddGridTextColumn(
                    L"Grade",
                    L"Grade",
                    90
                );

            gradeColumn->ReadOnly = true;

            AddGridTextColumn(
                L"GradePoint",
                L"Grade Point",
                100
            )->ReadOnly = true;

            AddGridTextColumn(
                L"Remarks",
                L"Remarks",
                220
            );
        }

        DataGridViewTextBoxColumn^ AddGridTextColumn(
            String^ name,
            String^ header,
            int width)
        {
            DataGridViewTextBoxColumn^ column =
                gcnew DataGridViewTextBoxColumn();

            column->Name = name;
            column->HeaderText = header;
            column->Width = width;
            column->SortMode =
                DataGridViewColumnSortMode::NotSortable;

            this->marksGrid->Columns->Add(column);

            return column;
        }

        void InitializeComponent()
        {
            this->SuspendLayout();

            this->Text =
                L"Enter Marks";

            this->StartPosition =
                FormStartPosition::CenterScreen;

            this->WindowState =
                FormWindowState::Maximized;

            this->MinimumSize =
                System::Drawing::Size(
                    1050,
                    700
                );

            this->BackColor =
                Color::FromArgb(
                    248,
                    250,
                    252
                );

            this->root =
                gcnew TableLayoutPanel();

            this->root->Dock =
                DockStyle::Fill;

            this->root->Padding =
                System::Windows::Forms::Padding(
                    20
                );

            this->root->ColumnCount = 1;
            this->root->RowCount = 4;

            this->root->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    88.0F
                )
            );

            this->root->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    88.0F
                )
            );

            this->root->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Percent,
                    100.0F
                )
            );

            this->root->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    58.0F
                )
            );

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
                System::Windows::Forms::Padding(
                    20,
                    8,
                    20,
                    8
                );

            this->lblTitle =
                gcnew Label();

            this->lblTitle->Dock =
                DockStyle::Top;

            this->lblTitle->Height = 42;

            this->lblTitle->Text =
                L"Enter Examination Marks";

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
                L"Select a subject and its examination paper.";

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

            this->filterPanel =
                gcnew Panel();

            this->filterPanel->Dock =
                DockStyle::Fill;

            this->filterPanel->BackColor =
                Color::White;

            this->filterPanel->Padding =
                System::Windows::Forms::Padding(
                    15,
                    10,
                    15,
                    10
                );

            TableLayoutPanel^ filters =
                gcnew TableLayoutPanel();

            filters->Dock =
                DockStyle::Fill;

            filters->ColumnCount = 8;
            filters->RowCount = 1;

            filters->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Absolute,
                    90.0F
                )
            );

            filters->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    33.0F
                )
            );

            filters->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Absolute,
                    70.0F
                )
            );

            filters->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    33.0F
                )
            );

            filters->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Absolute,
                    145.0F
                )
            );

            filters->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    17.0F
                )
            );

            filters->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    17.0F
                )
            );

            filters->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Absolute,
                    95.0F
                )
            );

            Label^ subjectLabel =
                gcnew Label();

            subjectLabel->Text = L"Subject";
            subjectLabel->Dock = DockStyle::Fill;
            subjectLabel->TextAlign =
                ContentAlignment::MiddleLeft;

            this->cmbSubject =
                gcnew ComboBox();

            this->cmbSubject->Dock =
                DockStyle::Fill;

            this->cmbSubject->DropDownStyle =
                ComboBoxStyle::DropDownList;

            Label^ paperLabel =
                gcnew Label();

            paperLabel->Text = L"Paper";
            paperLabel->Dock = DockStyle::Fill;
            paperLabel->TextAlign =
                ContentAlignment::MiddleLeft;

            this->cmbPaper =
                gcnew ComboBox();

            this->cmbPaper->Dock =
                DockStyle::Fill;

            this->cmbPaper->DropDownStyle =
                ComboBoxStyle::DropDownList;

            this->lblMaxScore =
                gcnew Label();

            this->lblMaxScore->Text =
                L"Maximum Score: —";

            this->lblMaxScore->Dock =
                DockStyle::Fill;

            this->lblMaxScore->TextAlign =
                ContentAlignment::MiddleCenter;

            this->lblPassMark =
                gcnew Label();

            this->lblPassMark->Text =
                L"Pass Mark: —";

            this->lblPassMark->Dock =
                DockStyle::Fill;

            this->lblPassMark->TextAlign =
                ContentAlignment::MiddleCenter;

            this->lblStudentCount =
                gcnew Label();

            this->lblStudentCount->Text =
                L"Students: 0";

            this->lblStudentCount->Dock =
                DockStyle::Fill;

            this->lblStudentCount->TextAlign =
                ContentAlignment::MiddleCenter;

            this->btnRefresh =
                gcnew Button();

            this->btnRefresh->Text =
                L"Refresh";

            this->btnRefresh->Dock =
                DockStyle::Fill;

            filters->Controls->Add(subjectLabel, 0, 0);
            filters->Controls->Add(this->cmbSubject, 1, 0);
            filters->Controls->Add(paperLabel, 2, 0);
            filters->Controls->Add(this->cmbPaper, 3, 0);
            filters->Controls->Add(this->lblMaxScore, 4, 0);
            filters->Controls->Add(this->lblPassMark, 5, 0);
            filters->Controls->Add(this->lblStudentCount, 6, 0);
            filters->Controls->Add(this->btnRefresh, 7, 0);

            this->filterPanel->Controls->Add(filters);

            ConfigureGrid();

            FlowLayoutPanel^ actionPanel =
                gcnew FlowLayoutPanel();

            actionPanel->Dock =
                DockStyle::Fill;

            actionPanel->FlowDirection =
                FlowDirection::RightToLeft;

            actionPanel->WrapContents = false;

            actionPanel->Padding =
                System::Windows::Forms::Padding(
                    0,
                    8,
                    0,
                    0
                );

            this->btnClose =
                gcnew Button();

            this->btnClose->Text =
                L"Close";

            this->btnClose->Size =
                System::Drawing::Size(
                    110,
                    38
                );

            this->btnSave =
                gcnew Button();

            this->btnSave->Text =
                L"Save Marks";

            this->btnSave->Size =
                System::Drawing::Size(
                    125,
                    38
                );

            actionPanel->Controls->Add(
                this->btnClose
            );

            actionPanel->Controls->Add(
                this->btnSave
            );

            this->root->Controls->Add(
                this->headerPanel,
                0,
                0
            );

            this->root->Controls->Add(
                this->filterPanel,
                0,
                1
            );

            this->root->Controls->Add(
                this->marksGrid,
                0,
                2
            );

            this->root->Controls->Add(
                actionPanel,
                0,
                3
            );

            this->Controls->Add(this->root);

            this->cmbSubject->SelectedIndexChanged +=
                gcnew EventHandler(
                    this,
                    &MarksEntryManagement::SubjectChanged
                );

            this->cmbPaper->SelectedIndexChanged +=
                gcnew EventHandler(
                    this,
                    &MarksEntryManagement::PaperChanged
                );

            this->btnRefresh->Click +=
                gcnew EventHandler(
                    this,
                    &MarksEntryManagement::RefreshClicked
                );

            this->btnSave->Click +=
                gcnew EventHandler(
                    this,
                    &MarksEntryManagement::SaveClicked
                );

            this->btnClose->Click +=
                gcnew EventHandler(
                    this,
                    &MarksEntryManagement::CloseClicked
                );

            this->ResumeLayout(false);
        }

    public:

        MarksEntryManagement(int selectedExaminationId)
        {
            this->examinationId =
                selectedExaminationId;

            InitializeComponent();

            ThemeManager::ApplyToForm(this);

            if (
                System::ComponentModel::LicenseManager::UsageMode
                != System::ComponentModel::LicenseUsageMode::Designtime)
            {
                LoadSubjects();
            }
        }

        ~MarksEntryManagement()
        {
        }
    };
}

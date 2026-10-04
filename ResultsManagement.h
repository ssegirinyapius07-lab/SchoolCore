#pragma once

#include "DbConnection.h"
#include "ThemeManager.h"

#include <mariadb/conncpp.hpp>
#include <memory>
#include <string>

using namespace System;
using namespace System::Drawing;
using namespace System::Windows::Forms;

namespace SchoolCore
{
    public ref class ResultsManagement : public Form
    {
    private:
        int examinationId = 0;

        TableLayoutPanel^ root;
        Panel^ headerPanel;
        Label^ lblTitle;
        Label^ lblSubtitle;
        Label^ lblSummary;
        DataGridView^ resultsGrid;
        Button^ btnRefresh;
        Button^ btnClose;

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
                L"Results",
                MessageBoxButtons::OK,
                MessageBoxIcon::Error
            );
        }

        void AddTextColumn(
            String^ name,
            String^ header,
            int width,
            bool readOnly)
        {
            DataGridViewTextBoxColumn^ column =
                gcnew DataGridViewTextBoxColumn();

            column->Name = name;
            column->HeaderText = header;
            column->Width = width;
            column->ReadOnly = readOnly;
            column->SortMode =
                DataGridViewColumnSortMode::NotSortable;

            this->resultsGrid->Columns->Add(column);
        }

        void LoadResults()
        {
            this->resultsGrid->Rows->Clear();
            this->lblSummary->Text =
                L"Results: 0";

            if (this->examinationId == 0)
                return;

            try
            {
                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT "
                        "s.registration_number, "
                        "CONCAT_WS(' ', s.first_name, s.middle_name, s.last_name) AS student_name, "
                        "COALESCE(st.stream_name, '') AS stream_name, "
                        "sub.subject_name, "
                        "ssr.total_score, "
                        "ssr.total_max_score, "
                        "ssr.percentage, "
                        "COALESCE(ssr.grade, '') AS grade, "
                        "ssr.grade_point, "
                        "ssr.grade_weight, "
                        "COALESCE(ssr.remarks, '') AS remarks, "
                        "ssr.paper_count, "
                        "(CASE "
                        " WHEN cl.academic_level_id = 2 THEN "
                        "   (SELECT COUNT(*) "
                        "    FROM subject_papers sp2 "
                        "    INNER JOIN curriculum_subjects cs2 "
                        "      ON cs2.curriculum_subject_id = sp2.curriculum_subject_id "
                        "    INNER JOIN curricula cur2 "
                        "      ON cur2.curriculum_id = cs2.curriculum_id "
                        "    WHERE cs2.subject_id = es.subject_id "
                        "      AND cs2.curriculum_id = ("
                        "          SELECT cur3.curriculum_id "
                        "          FROM curricula cur3 "
                        "          WHERE cur3.academic_level_id = cl.academic_level_id "
                        "            AND cur3.status = 'Active' "
                        "          ORDER BY cur3.effective_from_year DESC, cur3.curriculum_id DESC "
                        "          LIMIT 1"
                        "      ) "
                        "      AND cs2.status = 'Active' "
                        "      AND sp2.status = 'Active') "
                        " ELSE "
                        "   (SELECT COUNT(*) "
                        "    FROM examination_papers ep2 "
                        "    WHERE ep2.examination_subject_id = ssr.examination_subject_id "
                        "      AND ep2.status = 'Active') "
                        " END) AS required_papers, "
                        "(SELECT COUNT(*) "
                        " FROM marks m2 "
                        " INNER JOIN examination_papers ep3 "
                        " ON ep3.examination_paper_id = m2.examination_paper_id "
                        " WHERE ep3.examination_subject_id = ssr.examination_subject_id "
                        " AND ep3.status = 'Active' "
                        " AND m2.student_id = e.student_id) AS completed_papers "
                        "FROM student_subject_results ssr "
                        "INNER JOIN enrollments e "
                        "ON e.enrollment_id = ssr.enrollment_id "
                        "INNER JOIN students s "
                        "ON s.student_id = e.student_id "
                        "LEFT JOIN streams st "
                        "ON st.stream_id = e.stream_id "
                        "INNER JOIN examination_subjects es "
                        "ON es.examination_subject_id = ssr.examination_subject_id "
                        "INNER JOIN examinations ex "
                        "ON ex.examination_id = es.examination_id "
                        "INNER JOIN classes cl "
                        "ON cl.class_id = ex.class_id "
                        "INNER JOIN subjects sub "
                        "ON sub.subject_id = es.subject_id "
                        "WHERE es.examination_id = ? "
                        "ORDER BY "
                        "st.stream_name ASC, "
                        "s.first_name ASC, "
                        "s.middle_name ASC, "
                        "s.last_name ASC, "
                        "sub.subject_name ASC"
                    )
                );

                stmt->setInt(
                    1,
                    this->examinationId
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                int count = 0;

                while (result->next())
                {
                    int row =
                        this->resultsGrid->Rows->Add();

                    this->resultsGrid->Rows[row]
                        ->Cells["Registration"]->Value =
                        GetString(
                            result.get(),
                            "registration_number"
                        );

                    this->resultsGrid->Rows[row]
                        ->Cells["Student"]->Value =
                        GetString(
                            result.get(),
                            "student_name"
                        );

                    this->resultsGrid->Rows[row]
                        ->Cells["Stream"]->Value =
                        GetString(
                            result.get(),
                            "stream_name"
                        );

                    this->resultsGrid->Rows[row]
                        ->Cells["Subject"]->Value =
                        GetString(
                            result.get(),
                            "subject_name"
                        );

                    this->resultsGrid->Rows[row]
                        ->Cells["Score"]->Value =
                        result->getDouble(
                            "total_score"
                        ).ToString("0.##");

                    this->resultsGrid->Rows[row]
                        ->Cells["Maximum"]->Value =
                        result->getDouble(
                            "total_max_score"
                        ).ToString("0.##");

                    this->resultsGrid->Rows[row]
                        ->Cells["Percentage"]->Value =
                        result->getDouble(
                            "percentage"
                        ).ToString("0.##");

                    this->resultsGrid->Rows[row]
                        ->Cells["Grade"]->Value =
                        GetString(
                            result.get(),
                            "grade"
                        );

                    this->resultsGrid->Rows[row]
                        ->Cells["GradePoint"]->Value =
                        result->isNull("grade_point")
                        ? L""
                        : result->getDouble(
                              "grade_point"
                          ).ToString("0.##");

                    this->resultsGrid->Rows[row]
                        ->Cells["GradeWeight"]->Value =
                        result->isNull("grade_weight")
                        ? L""
                        : result->getDouble(
                              "grade_weight"
                          ).ToString("0.##");

                    this->resultsGrid->Rows[row]
                        ->Cells["Remarks"]->Value =
                        GetString(
                            result.get(),
                            "remarks"
                        );

                    int completedPapers =
                        result->getInt("completed_papers");

                    int requiredPapers =
                        result->getInt("required_papers");

                    this->resultsGrid->Rows[row]
                        ->Cells["Papers"]->Value =
                        completedPapers.ToString() +
                        L"/" +
                        requiredPapers.ToString();

                    this->resultsGrid->Rows[row]
                        ->Cells["Status"]->Value =
                        (
                            completedPapers < requiredPapers ||
                            String::IsNullOrWhiteSpace(
                                GetString(result.get(), "grade")
                            )
                        )
                        ? L"Pending"
                        : L"Calculated";

                    count++;
                }

                this->lblSummary->Text =
                    L"Subject results: " +
                    count.ToString() +
                    L" | Papers show completed/required";
            }
            catch (sql::SQLException& ex)
            {
                ShowDatabaseError(ex);
            }
        }

        void ConfigureGrid()
        {
            this->resultsGrid =
                gcnew DataGridView();

            this->resultsGrid->Dock =
                DockStyle::Fill;

            this->resultsGrid->ReadOnly = true;
            this->resultsGrid->AllowUserToAddRows = false;
            this->resultsGrid->AllowUserToDeleteRows = false;
            this->resultsGrid->AllowUserToResizeRows = false;
            this->resultsGrid->AutoGenerateColumns = false;
            this->resultsGrid->SelectionMode =
                DataGridViewSelectionMode::FullRowSelect;
            this->resultsGrid->MultiSelect = false;
            this->resultsGrid->RowHeadersVisible = false;
            this->resultsGrid->BackgroundColor = Color::White;
            this->resultsGrid->BorderStyle = BorderStyle::None;
            this->resultsGrid->ColumnHeadersHeight = 42;
            this->resultsGrid->RowTemplate->Height = 34;

            AddTextColumn(
                L"Registration",
                L"Registration",
                145,
                true
            );

            AddTextColumn(
                L"Student",
                L"Student",
                250,
                true
            );

            AddTextColumn(
                L"Stream",
                L"Stream",
                110,
                true
            );

            AddTextColumn(
                L"Subject",
                L"Subject",
                190,
                true
            );

            AddTextColumn(
                L"Score",
                L"Score",
                85,
                true
            );

            AddTextColumn(
                L"Maximum",
                L"Maximum",
                85,
                true
            );

            AddTextColumn(
                L"Percentage",
                L"%",
                75,
                true
            );

            AddTextColumn(
                L"Grade",
                L"Grade",
                75,
                true
            );

            AddTextColumn(
                L"GradePoint",
                L"Points",
                80,
                true
            );

            AddTextColumn(
                L"GradeWeight",
                L"Weight",
                80,
                true
            );

            AddTextColumn(
                L"Remarks",
                L"Remarks",
                180,
                true
            );

            AddTextColumn(
                L"Papers",
                L"Papers",
                85,
                true
            );

            AddTextColumn(
                L"Status",
                L"Status",
                95,
                true
            );
        }

        void RefreshClicked(
            Object^ sender,
            EventArgs^ e)
        {
            LoadResults();
        }

        void CloseClicked(
            Object^ sender,
            EventArgs^ e)
        {
            this->Close();
        }

        void InitializeComponent()
        {
            this->SuspendLayout();

            this->Text =
                L"Results";

            this->StartPosition =
                FormStartPosition::CenterScreen;

            this->WindowState =
                FormWindowState::Maximized;

            this->MinimumSize =
                System::Drawing::Size(
                    1100,
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
                    48.0F
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
                L"Examination Results";

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
                L"Completed subject results calculated from all configured papers.";

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

            this->lblSummary =
                gcnew Label();

            this->lblSummary->Dock =
                DockStyle::Fill;

            this->lblSummary->Text =
                L"Completed subject results: 0";

            this->lblSummary->TextAlign =
                ContentAlignment::MiddleLeft;

            this->lblSummary->ForeColor =
                Color::DimGray;

            ConfigureGrid();

            FlowLayoutPanel^ actions =
                gcnew FlowLayoutPanel();

            actions->Dock =
                DockStyle::Fill;

            actions->FlowDirection =
                FlowDirection::RightToLeft;

            actions->WrapContents = false;

            actions->Padding =
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

            this->btnRefresh =
                gcnew Button();

            this->btnRefresh->Text =
                L"Refresh";

            this->btnRefresh->Size =
                System::Drawing::Size(
                    110,
                    38
                );

            actions->Controls->Add(
                this->btnClose
            );

            actions->Controls->Add(
                this->btnRefresh
            );

            this->root->Controls->Add(
                this->headerPanel,
                0,
                0
            );

            this->root->Controls->Add(
                this->lblSummary,
                0,
                1
            );

            this->root->Controls->Add(
                this->resultsGrid,
                0,
                2
            );

            this->root->Controls->Add(
                actions,
                0,
                3
            );

            this->Controls->Add(
                this->root
            );

            this->btnRefresh->Click +=
                gcnew EventHandler(
                    this,
                    &ResultsManagement::RefreshClicked
                );

            this->btnClose->Click +=
                gcnew EventHandler(
                    this,
                    &ResultsManagement::CloseClicked
                );

            this->ResumeLayout(false);
        }

    public:

        ResultsManagement(int selectedExaminationId)
        {
            this->examinationId =
                selectedExaminationId;

            InitializeComponent();

            ThemeManager::ApplyToForm(this);

            if (
                System::ComponentModel::LicenseManager::UsageMode
                != System::ComponentModel::LicenseUsageMode::Designtime)
            {
                LoadResults();
            }
        }

        ~ResultsManagement()
        {
        }
    };
}

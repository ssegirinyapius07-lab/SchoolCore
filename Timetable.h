#pragma once

#include "DbConnection.h"

#include <mariadb/conncpp.hpp>
#include <msclr/marshal_cppstd.h>

#include <memory>
#include <string>
#include <stdexcept>

using namespace System;
using namespace System::ComponentModel;
using namespace System::Drawing;
using namespace System::Windows::Forms;

namespace SchoolCore
{
    public ref class Timetable : public System::Windows::Forms::Form
    {
    public:
                Timetable()
                {
                    InitializeComponent();
                    LoadTimetable();
                }

        System::ComponentModel::Container^ components;


    protected:
        ~Timetable()
        {
            if (this->components)
            {
                delete this->components;
            }
        }

    private:

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

        System::Windows::Forms::TableLayoutPanel^ mainLayout;
        System::Windows::Forms::Panel^ headerPanel;
        System::Windows::Forms::Label^ lblTitle;
        System::Windows::Forms::Label^ lblSubtitle;

        System::Windows::Forms::ComboBox^ cmbYearFilter;
        System::Windows::Forms::ComboBox^ cmbTermFilter;
        System::Windows::Forms::ComboBox^ cmbClassFilter;
        System::Windows::Forms::ComboBox^ cmbDayFilter;
        System::Windows::Forms::Button^ btnRefresh;

        System::Windows::Forms::DataGridView^ timetableGrid;
        System::Windows::Forms::Button^ btnAddEntry;
        System::Windows::Forms::Button^ btnToggleEntry;
        System::Windows::Forms::Button^ btnBack;

        Form^ entryDialog;
        System::Windows::Forms::ComboBox^ entryYear;
        System::Windows::Forms::ComboBox^ entryTerm;
        System::Windows::Forms::ComboBox^ entryClass;
        System::Windows::Forms::ComboBox^ entryStream;
        System::Windows::Forms::ComboBox^ entrySubject;
        System::Windows::Forms::ComboBox^ entryTeacher;
        System::Windows::Forms::ComboBox^ entryDay;
        System::Windows::Forms::DateTimePicker^ entryStart;
        System::Windows::Forms::DateTimePicker^ entryEnd;
        System::Windows::Forms::TextBox^ entryRoom;
        System::Windows::Forms::TextBox^ entryNotes;

        void StyleGrid()
        {
            this->timetableGrid->BackgroundColor = Color::White;
            this->timetableGrid->BorderStyle = BorderStyle::None;
            this->timetableGrid->EnableHeadersVisualStyles = false;
            this->timetableGrid->ColumnHeadersDefaultCellStyle->BackColor =
                Color::FromArgb(30, 41, 59);
            this->timetableGrid->ColumnHeadersDefaultCellStyle->ForeColor =
                Color::White;
            this->timetableGrid->ColumnHeadersDefaultCellStyle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold", 9.0F, FontStyle::Bold);
            this->timetableGrid->ColumnHeadersHeight = 38;
            this->timetableGrid->DefaultCellStyle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI", 9.5F, FontStyle::Regular);
            this->timetableGrid->DefaultCellStyle->SelectionBackColor =
                Color::FromArgb(219, 234, 254);
            this->timetableGrid->DefaultCellStyle->SelectionForeColor =
                Color::FromArgb(30, 41, 59);
            this->timetableGrid->AlternatingRowsDefaultCellStyle->BackColor =
                Color::FromArgb(248, 250, 252);
            this->timetableGrid->RowTemplate->Height = 34;
            this->timetableGrid->RowHeadersVisible = false;
        }

        System::Windows::Forms::ComboBox^ CreateCombo()
        {
            System::Windows::Forms::ComboBox^ combo = gcnew ComboBox();
            combo->Dock = DockStyle::Fill;
            combo->DropDownStyle = ComboBoxStyle::DropDownList;
            combo->Margin = System::Windows::Forms::Padding(3);
            return combo;
        }

        System::Windows::Forms::Label^ CreateLabel(String^ text)
        {
            System::Windows::Forms::Label^ label = gcnew Label();
            label->Text = text;
            label->Dock = DockStyle::Fill;
            label->TextAlign = ContentAlignment::MiddleLeft;
            label->Font = gcnew System::Drawing::Font(
                L"Segoe UI", 9.5F, FontStyle::Bold);
            label->ForeColor = Color::FromArgb(71, 85, 105);
            return label;
        }

        void AddFilterRow(
            System::Windows::Forms::TableLayoutPanel^ layout,
            int column,
            String^ labelText,
            Control^ control)
        {
            layout->Controls->Add(
                CreateLabel(labelText),
                column,
                0
            );

            layout->Controls->Add(
                control,
                column,
                1
            );
        }

        void LoadYears(System::Windows::Forms::ComboBox^ combo)
        {
            combo->Items->Clear();

            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT academic_year_id, year_name "
                        "FROM academic_years "
                        "ORDER BY academic_year_id DESC"
                    )
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                while (result->next())
                {
                    combo->Items->Add(
                        gcnew ComboItem(
                            result->getInt("academic_year_id"),
                            gcnew String(
                                result->getString("year_name").c_str()
                            )
                        )
                    );
                }

                if (combo->Items->Count > 0)
                    combo->SelectedIndex = 0;
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

        void LoadTerms(System::Windows::Forms::ComboBox^ combo, int academicYearId)
        {
            combo->Items->Clear();

            if (academicYearId <= 0)
                return;

            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT term_id, term_name "
                        "FROM terms "
                        "WHERE academic_year_id = ? "
                        "ORDER BY term_id"
                    )
                );

                stmt->setInt(1, academicYearId);

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                while (result->next())
                {
                    combo->Items->Add(
                        gcnew ComboItem(
                            result->getInt("term_id"),
                            gcnew String(
                                result->getString("term_name").c_str()
                            )
                        )
                    );
                }

                if (combo->Items->Count > 0)
                    combo->SelectedIndex = 0;
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

        void LoadClasses(System::Windows::Forms::ComboBox^ combo, bool includeAll)
        {
            combo->Items->Clear();

            if (includeAll)
                combo->Items->Add(gcnew ComboItem(0, L"All Classes"));

            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT class_id, class_name "
                        "FROM classes "
                        "WHERE status = 'Active' "
                        "ORDER BY class_id"
                    )
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                while (result->next())
                {
                    combo->Items->Add(
                        gcnew ComboItem(
                            result->getInt("class_id"),
                            gcnew String(
                                result->getString("class_name").c_str()
                            )
                        )
                    );
                }

                if (combo->Items->Count > 0)
                    combo->SelectedIndex = 0;
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

        void LoadStreams(System::Windows::Forms::ComboBox^ combo, int classId)
        {
            combo->Items->Clear();
            combo->Items->Add(gcnew ComboItem(0, L"All Streams"));

            if (classId <= 0)
            {
                combo->SelectedIndex = 0;
                return;
            }

            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT stream_id, stream_name "
                        "FROM streams "
                        "WHERE class_id = ? AND status = 'Active' "
                        "ORDER BY stream_name"
                    )
                );

                stmt->setInt(1, classId);

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                while (result->next())
                {
                    combo->Items->Add(
                        gcnew ComboItem(
                            result->getInt("stream_id"),
                            gcnew String(
                                result->getString("stream_name").c_str()
                            )
                        )
                    );
                }

                combo->SelectedIndex = 0;
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

        void LoadSubjects(System::Windows::Forms::ComboBox^ combo)
        {
            combo->Items->Clear();

            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT subject_id, subject_code, subject_name "
                        "FROM subjects "
                        "WHERE status = 'Active' "
                        "ORDER BY subject_name"
                    )
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                while (result->next())
                {
                    String^ code = gcnew String(
                        result->getString("subject_code").c_str()
                    );
                    String^ name = gcnew String(
                        result->getString("subject_name").c_str()
                    );

                    combo->Items->Add(
                        gcnew ComboItem(
                            result->getInt("subject_id"),
                            code + L" - " + name
                        )
                    );
                }

                if (combo->Items->Count > 0)
                    combo->SelectedIndex = 0;
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

        void LoadTeachers(System::Windows::Forms::ComboBox^ combo)
        {
            combo->Items->Clear();
            combo->Items->Add(gcnew ComboItem(0, L"Unassigned"));

            try
            {
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT teacher_id, staff_number, "
                        "first_name, last_name "
                        "FROM teachers "
                        "WHERE employment_status = 'Active' "
                        "ORDER BY first_name, last_name"
                    )
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                while (result->next())
                {
                    String^ staff = gcnew String(
                        result->getString("staff_number").c_str()
                    );
                    String^ first = gcnew String(
                        result->getString("first_name").c_str()
                    );
                    String^ last = gcnew String(
                        result->getString("last_name").c_str()
                    );

                    combo->Items->Add(
                        gcnew ComboItem(
                            result->getInt("teacher_id"),
                            staff + L" - " + first + L" " + last
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

            if (combo->Items->Count > 0)
                combo->SelectedIndex = 0;
        }

        int SelectedId(System::Windows::Forms::ComboBox^ combo)
        {
            if (combo == nullptr ||
                combo->SelectedItem == nullptr)
                return 0;

            ComboItem^ item =
                dynamic_cast<ComboItem^>(combo->SelectedItem);

            return item == nullptr ? 0 : item->Id;
        }

        String^ SelectedText(System::Windows::Forms::ComboBox^ combo)
        {
            if (combo == nullptr ||
                combo->SelectedItem == nullptr)
                return L"";

            ComboItem^ item =
                dynamic_cast<ComboItem^>(combo->SelectedItem);

            return item == nullptr ? L"" : item->Text;
        }

        void LoadTimetable()
        {
            this->timetableGrid->Rows->Clear();

            int yearId = SelectedId(this->cmbYearFilter);
            int termId = SelectedId(this->cmbTermFilter);
            int classId = SelectedId(this->cmbClassFilter);
            String^ day = SelectedText(this->cmbDayFilter);

            try
            {
                auto con = DbConnection::GetConnection();

                String^ query =
                    L"SELECT "
                    L"te.timetable_entry_id, "
                    L"ay.year_name, "
                    L"t.term_name, "
                    L"c.class_name, "
                    L"COALESCE(st.stream_name, 'All Streams') AS stream_name, "
                    L"s.subject_code, "
                    L"s.subject_name, "
                    L"COALESCE(CONCAT(th.first_name, ' ', th.last_name), 'Unassigned') AS teacher_name, "
                    L"te.day_of_week, "
                    L"TIME_FORMAT(te.start_time, '%H:%i') AS start_time, "
                    L"TIME_FORMAT(te.end_time, '%H:%i') AS end_time, "
                    L"COALESCE(te.room, '') AS room, "
                    L"te.status "
                    L"FROM timetable_entries te "
                    L"INNER JOIN academic_years ay ON ay.academic_year_id = te.academic_year_id "
                    L"INNER JOIN terms t ON t.term_id = te.term_id "
                    L"INNER JOIN classes c ON c.class_id = te.class_id "
                    L"LEFT JOIN streams st ON st.stream_id = te.stream_id "
                    L"INNER JOIN subjects s ON s.subject_id = te.subject_id "
                    L"LEFT JOIN teachers th ON th.teacher_id = te.teacher_id "
                    L"WHERE 1 = 1 ";

                std::string sqlQuery =
                    msclr::interop::marshal_as<std::string>(query);

                if (yearId > 0)
                    sqlQuery += " AND te.academic_year_id = ? ";
                if (termId > 0)
                    sqlQuery += " AND te.term_id = ? ";
                if (classId > 0)
                    sqlQuery += " AND te.class_id = ? ";
                if (!String::IsNullOrWhiteSpace(day) &&
                    !day->Equals(L"All Days"))
                    sqlQuery += " AND te.day_of_week = ? ";

                sqlQuery +=
                    " ORDER BY FIELD(te.day_of_week, "
                    "'Monday','Tuesday','Wednesday','Thursday','Friday','Saturday'), "
                    "te.start_time, c.class_name, s.subject_name";

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(sqlQuery)
                );

                int index = 1;

                if (yearId > 0)
                    stmt->setInt(index++, yearId);
                if (termId > 0)
                    stmt->setInt(index++, termId);
                if (classId > 0)
                    stmt->setInt(index++, classId);
                if (!String::IsNullOrWhiteSpace(day) &&
                    !day->Equals(L"All Days"))
                {
                    stmt->setString(
                        index++,
                        msclr::interop::marshal_as<std::string>(day)
                    );
                }

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                while (result->next())
                {
                    this->timetableGrid->Rows->Add(
                        result->getInt("timetable_entry_id"),
                        gcnew String(result->getString("year_name").c_str()),
                        gcnew String(result->getString("term_name").c_str()),
                        gcnew String(result->getString("class_name").c_str()),
                        gcnew String(result->getString("stream_name").c_str()),
                        gcnew String(result->getString("subject_code").c_str()),
                        gcnew String(result->getString("subject_name").c_str()),
                        gcnew String(result->getString("teacher_name").c_str()),
                        gcnew String(result->getString("day_of_week").c_str()),
                        gcnew String(result->getString("start_time").c_str()),
                        gcnew String(result->getString("end_time").c_str()),
                        gcnew String(result->getString("room").c_str()),
                        gcnew String(result->getString("status").c_str())
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


        void EntryYearChanged(Object^ sender, EventArgs^ e)
        {
            ComboItem^ item =
                dynamic_cast<ComboItem^>(this->entryYear->SelectedItem);

            if (item != nullptr)
                LoadTerms(this->entryTerm, item->Id);
        }

        void EntryClassChanged(Object^ sender, EventArgs^ e)
        {
            ComboItem^ item =
                dynamic_cast<ComboItem^>(this->entryClass->SelectedItem);

            if (item != nullptr)
                LoadStreams(this->entryStream, item->Id);
        }

        void SaveEntry(Object^ sender, EventArgs^ e)
        {
            if (this->entryYear->SelectedItem == nullptr ||
                this->entryTerm->SelectedItem == nullptr ||
                this->entryClass->SelectedItem == nullptr ||
                this->entrySubject->SelectedItem == nullptr)
            {
                MessageBox::Show(
                    L"Academic year, term, class and subject are required.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                return;
            }

            TimeSpan startTime =
                this->entryStart->Value.TimeOfDay;
            TimeSpan endTime =
                this->entryEnd->Value.TimeOfDay;

            if (endTime <= startTime)
            {
                MessageBox::Show(
                    L"End time must be later than start time.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                return;
            }

            ComboItem^ yearItem =
                safe_cast<ComboItem^>(this->entryYear->SelectedItem);
            ComboItem^ termItem =
                safe_cast<ComboItem^>(this->entryTerm->SelectedItem);
            ComboItem^ classItem =
                safe_cast<ComboItem^>(this->entryClass->SelectedItem);
            ComboItem^ streamItem =
                dynamic_cast<ComboItem^>(this->entryStream->SelectedItem);
            ComboItem^ subjectItem =
                safe_cast<ComboItem^>(this->entrySubject->SelectedItem);
            ComboItem^ teacherItem =
                dynamic_cast<ComboItem^>(this->entryTeacher->SelectedItem);

            try
            {
                auto con = DbConnection::GetConnection();

                std::string sql =
                    "INSERT INTO timetable_entries "
                    "(academic_year_id, term_id, class_id, stream_id, "
                    "subject_id, teacher_id, day_of_week, start_time, "
                    "end_time, room, status, notes) "
                    "VALUES (?, ?, ?, ";

                if (streamItem != nullptr && streamItem->Id > 0)
                    sql += "?, ";
                else
                    sql += "NULL, ";

                sql += "?, ";

                if (teacherItem != nullptr && teacherItem->Id > 0)
                    sql += "?, ";
                else
                    sql += "NULL, ";

                sql += "?, ?, ?, ?, 'Active', ?)";

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(sql)
                );

                int p = 1;
                stmt->setInt(p++, yearItem->Id);
                stmt->setInt(p++, termItem->Id);
                stmt->setInt(p++, classItem->Id);

                if (streamItem != nullptr && streamItem->Id > 0)
                    stmt->setInt(p++, streamItem->Id);

                stmt->setInt(p++, subjectItem->Id);

                if (teacherItem != nullptr && teacherItem->Id > 0)
                    stmt->setInt(p++, teacherItem->Id);

                stmt->setString(
                    p++,
                    msclr::interop::marshal_as<std::string>(
                        safe_cast<String^>(this->entryDay->SelectedItem)
                    )
                );

                stmt->setString(
                    p++,
                    msclr::interop::marshal_as<std::string>(
                        startTime.ToString(L"hh\\:mm")
                    )
                );

                stmt->setString(
                    p++,
                    msclr::interop::marshal_as<std::string>(
                        endTime.ToString(L"hh\\:mm")
                    )
                );

                stmt->setString(
                    p++,
                    msclr::interop::marshal_as<std::string>(
                        this->entryRoom->Text->Trim()
                    )
                );

                stmt->setString(
                    p++,
                    msclr::interop::marshal_as<std::string>(
                        this->entryNotes->Text->Trim()
                    )
                );

                stmt->executeUpdate();

                MessageBox::Show(
                    L"Timetable entry added successfully.",
                    L"Timetable",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information
                );

                this->entryDialog->DialogResult =
                    System::Windows::Forms::DialogResult::OK;
                this->entryDialog->Close();
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

        void OpenEntryEditor()
        {
            this->entryDialog = gcnew Form();

            this->entryDialog->Text = L"Add Timetable Entry";
            this->entryDialog->StartPosition = FormStartPosition::CenterParent;
            this->entryDialog->FormBorderStyle =
                System::Windows::Forms::FormBorderStyle::FixedDialog;
            this->entryDialog->MaximizeBox = false;
            this->entryDialog->MinimizeBox = false;
            this->entryDialog->ShowInTaskbar = false;
            this->entryDialog->ClientSize = System::Drawing::Size(760, 610);
            this->entryDialog->BackColor = Color::FromArgb(248, 250, 252);

            System::Windows::Forms::Panel^ header = gcnew Panel();
            header->Dock = DockStyle::Top;
            header->Height = 82;
            header->BackColor = Color::FromArgb(30, 41, 59);
            header->Padding =
                System::Windows::Forms::Padding(20, 10, 20, 8);

            System::Windows::Forms::Label^ title = gcnew Label();
            title->Text = L"Add Timetable Entry";
            title->Dock = DockStyle::Top;
            title->Height = 34;
            title->Font = gcnew System::Drawing::Font(
                L"Segoe UI Semibold", 17.0F, FontStyle::Bold);
            title->ForeColor = Color::White;

            System::Windows::Forms::Label^ subtitle = gcnew Label();
            subtitle->Text =
                L"Assign a subject, teacher and time to a class.";
            subtitle->Dock = DockStyle::Fill;
            subtitle->Font = gcnew System::Drawing::Font(
                L"Segoe UI", 9.5F);
            subtitle->ForeColor = Color::Gainsboro;

            header->Controls->Add(subtitle);
            header->Controls->Add(title);

            System::Windows::Forms::TableLayoutPanel^ form = gcnew TableLayoutPanel();
            form->Dock = DockStyle::Fill;
            form->Padding =
                System::Windows::Forms::Padding(24, 18, 24, 12);
            form->ColumnCount = 4;
            form->RowCount = 6;

            form->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Absolute, 125.0F));
            form->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Percent, 50.0F));
            form->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Absolute, 105.0F));
            form->ColumnStyles->Add(
                gcnew ColumnStyle(SizeType::Percent, 50.0F));

            for (int i = 0; i < 6; i++)
                form->RowStyles->Add(
                    gcnew RowStyle(SizeType::Absolute, 54.0F));

            this->entryYear = CreateCombo();
            this->entryTerm = CreateCombo();
            this->entryClass = CreateCombo();
            this->entryStream = CreateCombo();
            this->entrySubject = CreateCombo();
            this->entryTeacher = CreateCombo();
            this->entryDay = CreateCombo();

            this->entryStart = gcnew DateTimePicker();
            this->entryEnd = gcnew DateTimePicker();
            this->entryRoom = gcnew TextBox();
            this->entryNotes = gcnew TextBox();

            this->entryStart->Dock = DockStyle::Fill;
            this->entryStart->Format = DateTimePickerFormat::Custom;
            this->entryStart->CustomFormat = L"HH:mm";
            this->entryStart->ShowUpDown = true;
            this->entryStart->Value = DateTime::Today.Date.AddHours(8);

            this->entryEnd->Dock = DockStyle::Fill;
            this->entryEnd->Format = DateTimePickerFormat::Custom;
            this->entryEnd->CustomFormat = L"HH:mm";
            this->entryEnd->ShowUpDown = true;
            this->entryEnd->Value = DateTime::Today.Date.AddHours(9);

            this->entryRoom->Dock = DockStyle::Fill;
            this->entryNotes->Dock = DockStyle::Fill;
            this->entryNotes->Multiline = true;

            this->entryDay->Items->Add(L"Monday");
            this->entryDay->Items->Add(L"Tuesday");
            this->entryDay->Items->Add(L"Wednesday");
            this->entryDay->Items->Add(L"Thursday");
            this->entryDay->Items->Add(L"Friday");
            this->entryDay->Items->Add(L"Saturday");
            this->entryDay->SelectedIndex = 0;

            LoadYears(this->entryYear);
            LoadClasses(this->entryClass, false);
            LoadSubjects(this->entrySubject);
            LoadTeachers(this->entryTeacher);

            if (this->entryYear->Items->Count > 0)
                LoadTerms(
                    this->entryTerm,
                    safe_cast<ComboItem^>(this->entryYear->SelectedItem)->Id
                );

            if (this->entryClass->Items->Count > 0)
                LoadStreams(
                    this->entryStream,
                    safe_cast<ComboItem^>(this->entryClass->SelectedItem)->Id
                );
            else
                this->entryStream->Items->Add(
                    gcnew ComboItem(0, L"All Streams")
                );

            form->Controls->Add(CreateLabel(L"Academic Year"), 0, 0);
            form->Controls->Add(this->entryYear, 1, 0);
            form->Controls->Add(CreateLabel(L"Term"), 2, 0);
            form->Controls->Add(this->entryTerm, 3, 0);

            form->Controls->Add(CreateLabel(L"Class"), 0, 1);
            form->Controls->Add(this->entryClass, 1, 1);
            form->Controls->Add(CreateLabel(L"Stream"), 2, 1);
            form->Controls->Add(this->entryStream, 3, 1);

            form->Controls->Add(CreateLabel(L"Subject"), 0, 2);
            form->Controls->Add(this->entrySubject, 1, 2);
            form->Controls->Add(CreateLabel(L"Teacher"), 2, 2);
            form->Controls->Add(this->entryTeacher, 3, 2);

            form->Controls->Add(CreateLabel(L"Day"), 0, 3);
            form->Controls->Add(this->entryDay, 1, 3);
            form->Controls->Add(CreateLabel(L"Start Time"), 2, 3);
            form->Controls->Add(this->entryStart, 3, 3);

            form->Controls->Add(CreateLabel(L"End Time"), 0, 4);
            form->Controls->Add(this->entryEnd, 1, 4);
            form->Controls->Add(CreateLabel(L"Room"), 2, 4);
            form->Controls->Add(this->entryRoom, 3, 4);

            form->Controls->Add(CreateLabel(L"Notes"), 0, 5);
            form->Controls->Add(this->entryNotes, 1, 5);
            form->SetColumnSpan(this->entryNotes, 3);

            System::Windows::Forms::Panel^ footer = gcnew Panel();
            footer->Dock = DockStyle::Bottom;
            footer->Height = 60;
            footer->Padding =
                System::Windows::Forms::Padding(24, 8, 24, 10);

            System::Windows::Forms::Button^ cancel = gcnew Button();
            cancel->Text = L"Cancel";
            cancel->Dock = DockStyle::Right;
            cancel->Width = 110;
            cancel->Height = 38;
            cancel->DialogResult =
                System::Windows::Forms::DialogResult::Cancel;

            System::Windows::Forms::Button^ save = gcnew Button();
            save->Text = L"Save Entry";
            save->Dock = DockStyle::Right;
            save->Width = 125;
            save->Height = 38;
            save->BackColor = Color::FromArgb(30, 41, 59);
            save->ForeColor = Color::White;
            save->FlatStyle = FlatStyle::Flat;
            save->FlatAppearance->BorderSize = 0;
            save->Margin =
                System::Windows::Forms::Padding(0, 0, 10, 0);

            footer->Controls->Add(cancel);
            footer->Controls->Add(save);

            this->entryDialog->Controls->Add(form);
            this->entryDialog->Controls->Add(footer);
            this->entryDialog->Controls->Add(header);

            this->entryYear->SelectedIndexChanged +=
                gcnew EventHandler(this, &Timetable::EntryYearChanged);

            this->entryClass->SelectedIndexChanged +=
                gcnew EventHandler(this, &Timetable::EntryClassChanged);

            save->Click +=
                gcnew EventHandler(this, &Timetable::SaveEntry);

            this->entryDialog->AcceptButton = save;
            this->entryDialog->CancelButton = cancel;

            if (this->entryDialog->ShowDialog(this) ==
                System::Windows::Forms::DialogResult::OK)
            {
                if (System::ComponentModel::LicenseManager::UsageMode != System::ComponentModel::LicenseUsageMode::Designtime)
            {
                LoadTimetable();
            }
            }
        }


        System::Void YearFilterChanged(
            Object^ sender,
            EventArgs^ e)
        {
            ComboItem^ item =
                dynamic_cast<ComboItem^>(
                    this->cmbYearFilter->SelectedItem);

            if (item != nullptr)
                LoadTerms(
                    this->cmbTermFilter,
                    item->Id
                );

            LoadTimetable();
        }

        System::Void GridSelectionChanged(
            Object^ sender,
            EventArgs^ e)
        {
            this->btnToggleEntry->Enabled =
                this->timetableGrid->SelectedRows->Count > 0;
        }

        System::Void FilterChanged(
            Object^ sender,
            EventArgs^ e)
        {
            LoadTimetable();
        }

        System::Void btnAddEntry_Click(
            Object^ sender,
            EventArgs^ e)
        {
            OpenEntryEditor();
        }

        System::Void btnToggleEntry_Click(
            Object^ sender,
            EventArgs^ e)
        {
            if (this->timetableGrid->SelectedRows->Count == 0)
            {
                MessageBox::Show(
                    L"Please select a timetable entry.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                return;
            }

            DataGridViewRow^ row =
                this->timetableGrid->SelectedRows[0];

            int id = Convert::ToInt32(
                row->Cells[L"EntryId"]->Value
            );

            String^ currentStatus =
                Convert::ToString(
                    row->Cells[L"Status"]->Value
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
                auto con = DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "UPDATE timetable_entries "
                        "SET status = ? "
                        "WHERE timetable_entry_id = ?"
                    )
                );

                stmt->setString(
                    1,
                    msclr::interop::marshal_as<std::string>(
                        newStatus
                    )
                );

                stmt->setInt(2, id);
                stmt->executeUpdate();

                LoadTimetable();
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

        System::Void btnBack_Click(
            Object^ sender,
            EventArgs^ e)
        {
            this->Close();
        }

        #pragma region Windows Form Designer generated code

void InitializeComponent()
        {
            this->components = gcnew System::ComponentModel::Container();
            this->SuspendLayout();

            this->Text = L"Timetable";
            this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
            this->WindowState = System::Windows::Forms::FormWindowState::Normal;
            this->ClientSize = System::Drawing::Size(1120, 650);
            this->MinimumSize = System::Drawing::Size(1000, 600);
            this->BackColor = System::Drawing::Color::FromArgb(248, 250, 252);
            this->Font = gcnew System::Drawing::Font(
                L"Segoe UI", 9.5F);

            this->mainLayout = gcnew System::Windows::Forms::TableLayoutPanel();
            this->mainLayout->Dock = System::Windows::Forms::DockStyle::Fill;
            this->mainLayout->AutoScroll = true;
            this->mainLayout->ColumnCount = 1;
            this->mainLayout->RowCount = 4;
            this->mainLayout->Padding =
                System::Windows::Forms::Padding(18);
            this->mainLayout->BackColor =
                System::Drawing::Color::FromArgb(248, 250, 252);

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(System::Windows::Forms::SizeType::Absolute, 78.0F));
            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(System::Windows::Forms::SizeType::Absolute, 70.0F));
            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(System::Windows::Forms::SizeType::Percent, 100.0F));
            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(System::Windows::Forms::SizeType::Absolute, 58.0F));

            this->headerPanel = gcnew System::Windows::Forms::Panel();
            this->headerPanel->Dock = System::Windows::Forms::DockStyle::Fill;
            this->headerPanel->BackColor =
                System::Drawing::Color::FromArgb(35, 47, 62);
            this->headerPanel->Padding =
                System::Windows::Forms::Padding(20, 9, 20, 8);

            this->lblTitle = gcnew System::Windows::Forms::Label();
            this->lblTitle->Text = L"Timetable";
            this->lblTitle->Dock = System::Windows::Forms::DockStyle::Top;
            this->lblTitle->Height = 35;
            this->lblTitle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold", 17.0F, System::Drawing::FontStyle::Bold);
            this->lblTitle->ForeColor = System::Drawing::Color::White;

            this->lblSubtitle = gcnew System::Windows::Forms::Label();
            this->lblSubtitle->Text =
                L"Plan lessons by academic year, class, stream, subject and teacher.";
            this->lblSubtitle->Dock = System::Windows::Forms::DockStyle::Fill;
            this->lblSubtitle->ForeColor = System::Drawing::Color::Gainsboro;
            this->lblSubtitle->Font =
                gcnew System::Drawing::Font(L"Segoe UI", 9.5F);

            this->headerPanel->Controls->Add(this->lblSubtitle);
            this->headerPanel->Controls->Add(this->lblTitle);

            System::Windows::Forms::TableLayoutPanel^ filters = gcnew System::Windows::Forms::TableLayoutPanel();
            filters->Dock = System::Windows::Forms::DockStyle::Fill;
            filters->ColumnCount = 5;
            filters->RowCount = 2;
            filters->BackColor = System::Drawing::Color::White;
            filters->Padding =
                System::Windows::Forms::Padding(12, 7, 12, 7);

            for (int i = 0; i < 4; i++)
                filters->ColumnStyles->Add(
                    gcnew ColumnStyle(System::Windows::Forms::SizeType::Percent, 22.0F));
            filters->ColumnStyles->Add(
                gcnew ColumnStyle(System::Windows::Forms::SizeType::Percent, 12.0F));

            filters->RowStyles->Add(
                gcnew RowStyle(System::Windows::Forms::SizeType::Absolute, 22.0F));
            filters->RowStyles->Add(
                gcnew RowStyle(System::Windows::Forms::SizeType::Percent, 100.0F));

            this->cmbYearFilter = CreateCombo();
            this->cmbTermFilter = CreateCombo();
            this->cmbClassFilter = CreateCombo();
            this->cmbDayFilter = CreateCombo();

            this->cmbDayFilter->Items->Add(L"All Days");
            this->cmbDayFilter->Items->Add(L"Monday");
            this->cmbDayFilter->Items->Add(L"Tuesday");
            this->cmbDayFilter->Items->Add(L"Wednesday");
            this->cmbDayFilter->Items->Add(L"Thursday");
            this->cmbDayFilter->Items->Add(L"Friday");
            this->cmbDayFilter->Items->Add(L"Saturday");
            this->cmbDayFilter->SelectedIndex = 0;

            LoadYears(this->cmbYearFilter);
            LoadClasses(this->cmbClassFilter, true);

            if (this->cmbYearFilter->SelectedItem != nullptr)
            {
                ComboItem^ yearItem =
                    safe_cast<ComboItem^>(
                        this->cmbYearFilter->SelectedItem
                    );
                LoadTerms(this->cmbTermFilter, yearItem->Id);
            }

            AddFilterRow(filters, 0, L"Academic Year", this->cmbYearFilter);
            AddFilterRow(filters, 1, L"Term", this->cmbTermFilter);
            AddFilterRow(filters, 2, L"Class", this->cmbClassFilter);
            AddFilterRow(filters, 3, L"Day", this->cmbDayFilter);

            this->btnRefresh = gcnew System::Windows::Forms::Button();
            this->btnRefresh->Text = L"Refresh";
            this->btnRefresh->Dock = System::Windows::Forms::DockStyle::Fill;
            this->btnRefresh->Margin =
                System::Windows::Forms::Padding(3, 4, 3, 0);
            this->btnRefresh->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
            this->btnRefresh->BackColor =
                System::Drawing::Color::FromArgb(226, 232, 240);
            this->btnRefresh->ForeColor =
                System::Drawing::Color::FromArgb(30, 41, 59);
            this->btnRefresh->FlatAppearance->BorderSize = 0;

            filters->Controls->Add(this->btnRefresh, 4, 1);

            this->timetableGrid = gcnew System::Windows::Forms::DataGridView();
            this->timetableGrid->Dock = System::Windows::Forms::DockStyle::Fill;
            this->timetableGrid->AllowUserToAddRows = false;
            this->timetableGrid->AllowUserToDeleteRows = false;
            this->timetableGrid->AllowUserToResizeRows = false;
            this->timetableGrid->ReadOnly = true;
            this->timetableGrid->MultiSelect = false;
            this->timetableGrid->SelectionMode =
                System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
            this->timetableGrid->AutoSizeColumnsMode =
                System::Windows::Forms::DataGridViewAutoSizeColumnsMode::None;
            this->timetableGrid->AutoGenerateColumns = false;

            this->timetableGrid->Columns->Add(
                L"EntryId", L"ID");
            this->timetableGrid->Columns->Add(
                L"Year", L"Academic Year");
            this->timetableGrid->Columns->Add(
                L"Term", L"Term");
            this->timetableGrid->Columns->Add(
                L"Class", L"Class");
            this->timetableGrid->Columns->Add(
                L"Stream", L"Stream");
            this->timetableGrid->Columns->Add(
                L"SubjectCode", L"Code");
            this->timetableGrid->Columns->Add(
                L"Subject", L"Subject");
            this->timetableGrid->Columns->Add(
                L"Teacher", L"Teacher");
            this->timetableGrid->Columns->Add(
                L"Day", L"Day");
            this->timetableGrid->Columns->Add(
                L"Start", L"Start");
            this->timetableGrid->Columns->Add(
                L"End", L"End");
            this->timetableGrid->Columns->Add(
                L"Room", L"Room");
            this->timetableGrid->Columns->Add(
                L"Status", L"Status");

            this->timetableGrid->Columns[L"EntryId"]->Visible = false;
            this->timetableGrid->Columns[L"Year"]->Width = 105;
            this->timetableGrid->Columns[L"Term"]->Width = 105;
            this->timetableGrid->Columns[L"Class"]->Width = 105;
            this->timetableGrid->Columns[L"Stream"]->Width = 105;
            this->timetableGrid->Columns[L"SubjectCode"]->Width = 95;
            this->timetableGrid->Columns[L"Subject"]->Width = 145;
            this->timetableGrid->Columns[L"Teacher"]->Width = 155;
            this->timetableGrid->Columns[L"Day"]->Width = 90;
            this->timetableGrid->Columns[L"Start"]->Width = 75;
            this->timetableGrid->Columns[L"End"]->Width = 75;
            this->timetableGrid->Columns[L"Room"]->Width = 95;
            this->timetableGrid->Columns[L"Status"]->Width = 90;

            StyleGrid();

            System::Windows::Forms::FlowLayoutPanel^ footer = gcnew System::Windows::Forms::FlowLayoutPanel();
            footer->Dock = System::Windows::Forms::DockStyle::Fill;
            footer->FlowDirection = FlowDirection::RightToLeft;
            footer->WrapContents = false;
            footer->Padding =
                System::Windows::Forms::Padding(0, 8, 0, 0);

            this->btnBack = gcnew System::Windows::Forms::Button();
            this->btnBack->Text = L"Back to Dashboard";
            this->btnBack->Size = System::Drawing::Size(150, 40);

            this->btnToggleEntry = gcnew System::Windows::Forms::Button();
            this->btnToggleEntry->Text = L"Activate / Deactivate";
            this->btnToggleEntry->Size =
                System::Drawing::Size(155, 40);
            this->btnToggleEntry->Enabled = false;

            this->btnAddEntry = gcnew System::Windows::Forms::Button();
            this->btnAddEntry->Text = L"+ Add Entry";
            this->btnAddEntry->Size =
                System::Drawing::Size(125, 40);
            this->btnAddEntry->BackColor =
                System::Drawing::Color::FromArgb(30, 41, 59);
            this->btnAddEntry->ForeColor = System::Drawing::Color::White;
            this->btnAddEntry->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
            this->btnAddEntry->FlatAppearance->BorderSize = 0;

            footer->Controls->Add(this->btnBack);
            footer->Controls->Add(this->btnToggleEntry);
            footer->Controls->Add(this->btnAddEntry);

            this->mainLayout->Controls->Add(
                this->headerPanel, 0, 0);
            this->mainLayout->Controls->Add(
                filters, 0, 1);
            this->mainLayout->Controls->Add(
                this->timetableGrid, 0, 2);
            this->mainLayout->Controls->Add(
                footer, 0, 3);

            this->Controls->Add(this->mainLayout);

            this->cmbYearFilter->SelectedIndexChanged +=
                gcnew EventHandler(this, &Timetable::YearFilterChanged);

            this->cmbTermFilter->SelectedIndexChanged +=
                gcnew EventHandler(this, &Timetable::FilterChanged);

            this->cmbClassFilter->SelectedIndexChanged +=
                gcnew EventHandler(this, &Timetable::FilterChanged);

            this->cmbDayFilter->SelectedIndexChanged +=
                gcnew EventHandler(this, &Timetable::FilterChanged);

            this->btnRefresh->Click +=
                gcnew EventHandler(this, &Timetable::FilterChanged);

            this->timetableGrid->SelectionChanged +=
                gcnew EventHandler(this, &Timetable::GridSelectionChanged);

            this->btnAddEntry->Click +=
                gcnew EventHandler(
                    this,
                    &Timetable::btnAddEntry_Click
                );

            this->btnToggleEntry->Click +=
                gcnew EventHandler(
                    this,
                    &Timetable::btnToggleEntry_Click
                );

            this->btnBack->Click +=
                gcnew EventHandler(
                    this,
                    &Timetable::btnBack_Click
                );

            this->ResumeLayout(false);
        }

#pragma endregion

    public:


    };
}

#pragma once

#include "DbConnection.h"
#include "StudentManagement.h"
#include "TeacherManagement.h"
#include "AcademicYearsTerms.h"
#include "ClassesStreams.h"
#include "SubjectManagement.h"

namespace SchoolCore {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class Dashboard : public System::Windows::Forms::Form
	{
	public:
		Dashboard(void)
		{
			InitializeComponent();

			this->btnDashboard->Click +=
				gcnew System::EventHandler(
					this,
					&Dashboard::btnDashboard_Click
				);

			this->btnTeachers->Click +=
				gcnew System::EventHandler(
					this,
					&Dashboard::btnTeachers_Click
				);

			this->btnSubjects->Click +=
				gcnew System::EventHandler(
					this,
					&Dashboard::btnSubjects_Click
				);

			ShowDashboardOverview();
		}

	protected:
		~Dashboard()
		{
			if (components)
			{
				delete components;
			}
		}

	private:
		System::ComponentModel::Container^ components;

		// Dashboard panels
		System::Windows::Forms::Panel^ headerPanel;
		System::Windows::Forms::Panel^ sidebarPanel;
		System::Windows::Forms::Panel^ contentPanel;

		// Dashboard header labels
		System::Windows::Forms::Label^ lblSchoolName;
		System::Windows::Forms::Label^ lblSubtitle;
		// Sidebar navigation
		System::Windows::Forms::Button^ btnDashboard;
		System::Windows::Forms::Button^ btnStudents;
		System::Windows::Forms::Button^ btnTeachers;
		System::Windows::Forms::Button^ btnClasses;
		System::Windows::Forms::Button^ btnSubjects;
		System::Windows::Forms::Button^ btnAcademic;
		System::Windows::Forms::Button^ btnTimetable;
		System::Windows::Forms::Button^ btnAttendance;
		System::Windows::Forms::Button^ btnExaminations;
		System::Windows::Forms::Button^ btnFees;
		System::Windows::Forms::Button^ btnDiscipline;
		System::Windows::Forms::Button^ btnCommunication;
		System::Windows::Forms::Button^ btnReports;
		System::Windows::Forms::Button^ btnUsers;
		System::Windows::Forms::Button^ btnSettings;

		System::Void btnTeachers_Click(
			System::Object^ sender,
			System::EventArgs^ e)
		{
			TeacherManagement^ form =
				gcnew TeacherManagement();

			form->ShowDialog(this);
		}

		System::Void btnSubjects_Click(
			System::Object^ sender,
			System::EventArgs^ e)
		{
			SubjectManagement^ form =
				gcnew SubjectManagement();

			form->ShowDialog(this);
		}

		System::Void btnStudents_Click(
			System::Object^ sender,
			System::EventArgs^ e)
		{
			StudentManagement^ form =
				gcnew StudentManagement();

			form->ShowDialog(this);
		}

		System::Void btnClasses_Click(
			System::Object^ sender,
			System::EventArgs^ e)
		{
			this->contentPanel->Controls->Clear();

			ClassesStreams^ form =
				gcnew ClassesStreams();

			form->TopLevel = false;
			form->FormBorderStyle =
				System::Windows::Forms::FormBorderStyle::None;
			form->Dock =
				System::Windows::Forms::DockStyle::Fill;
			form->WindowState =
				System::Windows::Forms::FormWindowState::Normal;

			this->contentPanel->Controls->Add(form);

			form->Show();
		}

		System::Void btnAcademic_Click(
			System::Object^ sender,
			System::EventArgs^ e)
		{
			this->contentPanel->Controls->Clear();

			SchoolCore::AcademicYearsTerms^ form =
				gcnew SchoolCore::AcademicYearsTerms();

			form->TopLevel = false;
			form->FormBorderStyle =
				System::Windows::Forms::FormBorderStyle::None;
			form->Dock =
				System::Windows::Forms::DockStyle::Fill;
			form->WindowState =
				System::Windows::Forms::FormWindowState::Normal;

			this->contentPanel->Controls->Add(form);

			form->Show();
		}


		System::Void btnDashboard_Click(
			System::Object^ sender,
			System::EventArgs^ e)
		{
			ShowDashboardOverview();
		}


		void ShowDashboardOverview()
		{
			this->contentPanel->Controls->Clear();

			// Layout: title / stat cards / content area / footer
			TableLayoutPanel^ layout = gcnew TableLayoutPanel();
			layout->Dock = DockStyle::Fill;
			layout->Padding = System::Windows::Forms::Padding(30, 20, 30, 10);
			layout->ColumnCount = 4;
			layout->RowCount = 4;
			layout->BackColor = Color::White;

			for (int i = 0; i < 4; i++)
			{
				layout->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Percent, 25.0F));
			}

			layout->RowStyles->Add(gcnew RowStyle(SizeType::Absolute, 60.0F));   // title
			layout->RowStyles->Add(gcnew RowStyle(SizeType::Absolute, 120.0F));  // stat cards
			layout->RowStyles->Add(gcnew RowStyle(SizeType::Percent, 100.0F));   // free content area
			layout->RowStyles->Add(gcnew RowStyle(SizeType::Absolute, 40.0F));   // footer

			// Title
			Label^ title = gcnew Label();
			title->Text = L"Dashboard";
			title->Dock = DockStyle::Fill;
			title->Font = gcnew System::Drawing::Font(L"Segoe UI Semibold", 22.0F, FontStyle::Bold);
			title->ForeColor = Color::FromArgb(30, 41, 59);
			title->TextAlign = ContentAlignment::MiddleLeft;

			layout->Controls->Add(title, 0, 0);
			layout->SetColumnSpan(title, 4);

			// Live counts (same queries as before)
			int studentCount = 0;
			int teacherCount = 0;
			int classCount = 0;
			String^ currentAcademicYear = L"Not set";

			try
			{
				auto con = DbConnection::GetConnection();

				std::unique_ptr<sql::PreparedStatement> studentStmt(
					con->prepareStatement("SELECT COUNT(*) AS total FROM students WHERE status = 'Active'"));
				std::unique_ptr<sql::ResultSet> studentResult(studentStmt->executeQuery());
				if (studentResult->next()) studentCount = studentResult->getInt("total");

				std::unique_ptr<sql::PreparedStatement> teacherStmt(
					con->prepareStatement("SELECT COUNT(*) AS total FROM teachers WHERE employment_status = 'Active'"));
				std::unique_ptr<sql::ResultSet> teacherResult(teacherStmt->executeQuery());
				if (teacherResult->next()) teacherCount = teacherResult->getInt("total");

				std::unique_ptr<sql::PreparedStatement> classStmt(
					con->prepareStatement("SELECT COUNT(*) AS total FROM classes WHERE status = 'Active'"));
				std::unique_ptr<sql::ResultSet> classResult(classStmt->executeQuery());
				if (classResult->next()) classCount = classResult->getInt("total");
				std::unique_ptr<sql::PreparedStatement> yearStmt(
					con->prepareStatement(
						"SELECT year_name "
						"FROM academic_years "
						"WHERE status = 'Active' "
						"ORDER BY academic_year_id DESC "
						"LIMIT 1"
					)
				);

				std::unique_ptr<sql::ResultSet> yearResult(
					yearStmt->executeQuery()
				);

				if (yearResult->next())
				{
					currentAcademicYear =
						gcnew String(
							yearResult->getString("year_name").c_str()
						);
				}
			}
			catch (sql::SQLException& ex)
			{
				MessageBox::Show(gcnew String(ex.what()), L"Database Error",
					MessageBoxButtons::OK, MessageBoxIcon::Error);
			}

			// Stat cards: one row of four
			layout->Controls->Add(CreateDashboardCard(L"Students", studentCount.ToString(),
				Color::FromArgb(37, 99, 235), true), 0, 1);

			layout->Controls->Add(CreateDashboardCard(L"Teachers", teacherCount.ToString(),
				Color::FromArgb(16, 185, 129), true), 1, 1);

			layout->Controls->Add(CreateDashboardCard(L"Classes", classCount.ToString(),
				Color::FromArgb(245, 158, 11), true), 2, 1);

			layout->Controls->Add(CreateDashboardCard(L"Academic Year", currentAcademicYear,
				Color::FromArgb(139, 92, 246), false), 3, 1);

			// Free content area (two panels, ready for real content later)
			TableLayoutPanel^ lower = gcnew TableLayoutPanel();
			lower->Dock = DockStyle::Fill;
			lower->Margin = System::Windows::Forms::Padding(0, 20, 0, 0);
			lower->ColumnCount = 2;
			lower->RowCount = 1;
			lower->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Percent, 60.0F));
			lower->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Percent, 40.0F));

			lower->Controls->Add(CreateSectionPanel(L"Recent Activity",
				L"Latest enrolments and changes will appear here."), 0, 0);
			lower->Controls->Add(CreateSectionPanel(L"Quick Actions",
				L"Shortcuts for common tasks will appear here."), 1, 0);

			layout->Controls->Add(lower, 0, 2);
			layout->SetColumnSpan(lower, 4);

			// Footer
			Label^ footer = gcnew Label();
			footer->Text = L"SchoolCore  |  Secondary School Management System";
			footer->Dock = DockStyle::Fill;
			footer->ForeColor = Color::DimGray;
			footer->TextAlign = ContentAlignment::MiddleLeft;

			layout->Controls->Add(footer, 0, 3);
			layout->SetColumnSpan(footer, 4);

			this->contentPanel->Controls->Add(layout);
		}


		Panel^ CreateDashboardCard(String^ title, String^ value, Color accent, bool addGap)
		{
			Panel^ card = gcnew Panel();
			card->Dock = DockStyle::Fill;
			card->Margin = System::Windows::Forms::Padding(0, 0, addGap ? 15 : 0, 0);
			card->BackColor = Color::FromArgb(248, 250, 252);
			card->BorderStyle = BorderStyle::FixedSingle;

			// Coloured accent strip on the left edge
			Panel^ strip = gcnew Panel();
			strip->Dock = DockStyle::Left;
			strip->Width = 6;
			strip->BackColor = accent;

			Label^ titleLabel = gcnew Label();
			titleLabel->Text = title;
			titleLabel->AutoSize = true;
			titleLabel->Font = gcnew System::Drawing::Font(L"Segoe UI", 10.0F, FontStyle::Regular);
			titleLabel->ForeColor = Color::DimGray;
			titleLabel->Location = Point(24, 18);

			Label^ valueLabel = gcnew Label();
			valueLabel->Text = value;
			valueLabel->AutoSize = true;
			valueLabel->Font = gcnew System::Drawing::Font(L"Segoe UI Semibold", 26.0F, FontStyle::Bold);
			valueLabel->ForeColor = Color::FromArgb(30, 41, 59);
			valueLabel->Location = Point(22, 44);

			card->Controls->Add(titleLabel);
			card->Controls->Add(valueLabel);
			card->Controls->Add(strip);

			return card;
		}


		Panel^ CreateSectionPanel(String^ heading, String^ placeholder)
		{
			Panel^ section = gcnew Panel();
			section->Dock = DockStyle::Fill;
			section->Margin = System::Windows::Forms::Padding(0, 0, 15, 0);
			section->BackColor = Color::White;
			section->BorderStyle = BorderStyle::FixedSingle;
			section->Padding = System::Windows::Forms::Padding(20);

			Label^ headingLabel = gcnew Label();
			headingLabel->Text = heading;
			headingLabel->Dock = DockStyle::Top;
			headingLabel->Height = 34;
			headingLabel->Font = gcnew System::Drawing::Font(L"Segoe UI Semibold", 12.0F, FontStyle::Bold);
			headingLabel->ForeColor = Color::FromArgb(30, 41, 59);

			Label^ hint = gcnew Label();
			hint->Text = placeholder;
			hint->Dock = DockStyle::Fill;
			hint->Font = gcnew System::Drawing::Font(L"Segoe UI", 10.0F);
			hint->ForeColor = Color::Gray;
			hint->TextAlign = ContentAlignment::MiddleCenter;

			section->Controls->Add(hint);
			section->Controls->Add(headingLabel);

			return section;
		}


#pragma region Windows Form Designer generated code

		void InitializeComponent(void)
		{
			this->lblSchoolName = (gcnew System::Windows::Forms::Label());
			this->lblSubtitle = (gcnew System::Windows::Forms::Label());
			this->headerPanel = (gcnew System::Windows::Forms::Panel());
			this->sidebarPanel = (gcnew System::Windows::Forms::Panel());
			this->btnDashboard = (gcnew System::Windows::Forms::Button());
			this->btnStudents = (gcnew System::Windows::Forms::Button());
			this->btnTeachers = (gcnew System::Windows::Forms::Button());
			this->btnClasses = (gcnew System::Windows::Forms::Button());
			this->btnSubjects = (gcnew System::Windows::Forms::Button());
			this->btnAcademic = (gcnew System::Windows::Forms::Button());
			this->btnTimetable = (gcnew System::Windows::Forms::Button());
			this->btnAttendance = (gcnew System::Windows::Forms::Button());
			this->btnExaminations = (gcnew System::Windows::Forms::Button());
			this->btnFees = (gcnew System::Windows::Forms::Button());
			this->btnDiscipline = (gcnew System::Windows::Forms::Button());
			this->btnCommunication = (gcnew System::Windows::Forms::Button());
			this->btnReports = (gcnew System::Windows::Forms::Button());
			this->btnUsers = (gcnew System::Windows::Forms::Button());
			this->btnSettings = (gcnew System::Windows::Forms::Button());
			this->contentPanel = (gcnew System::Windows::Forms::Panel());
			this->headerPanel->SuspendLayout();
			this->sidebarPanel->SuspendLayout();
			this->SuspendLayout();
			// 
			// lblSchoolName
			// 
			this->lblSchoolName->Font = (gcnew System::Drawing::Font(L"Segoe UI", 22, System::Drawing::FontStyle::Bold));
			this->lblSchoolName->ForeColor = System::Drawing::Color::White;
			this->lblSchoolName->Location = System::Drawing::Point(25, 12);
			this->lblSchoolName->Name = L"lblSchoolName";
			this->lblSchoolName->Size = System::Drawing::Size(500, 48);
			this->lblSchoolName->TabIndex = 1;
			this->lblSchoolName->Text = L"SchoolCore";
			this->lblSchoolName->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->lblSchoolName->UseCompatibleTextRendering = true;
			// 
			// lblSubtitle
			// 
			this->lblSubtitle->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10));
			this->lblSubtitle->ForeColor = System::Drawing::Color::Gainsboro;
			this->lblSubtitle->Location = System::Drawing::Point(25, 60);
			this->lblSubtitle->Name = L"lblSubtitle";
			this->lblSubtitle->Size = System::Drawing::Size(600, 25);
			this->lblSubtitle->TabIndex = 0;
			this->lblSubtitle->Text = L"Secondary School Management System";
			this->lblSubtitle->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->lblSubtitle->UseCompatibleTextRendering = true;
			// 
			// headerPanel
			// 
			this->headerPanel->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(30)), static_cast<System::Int32>(static_cast<System::Byte>(41)),
				static_cast<System::Int32>(static_cast<System::Byte>(59)));
			this->headerPanel->Controls->Add(this->lblSubtitle);
			this->headerPanel->Controls->Add(this->lblSchoolName);
			this->headerPanel->Dock = System::Windows::Forms::DockStyle::Top;
			this->headerPanel->Location = System::Drawing::Point(0, 0);
			this->headerPanel->Name = L"headerPanel";
			this->headerPanel->Size = System::Drawing::Size(1309, 100);
			this->headerPanel->TabIndex = 0;
			// 
			// sidebarPanel
			// 
			this->sidebarPanel->AutoScroll = true;
			this->sidebarPanel->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(15)), static_cast<System::Int32>(static_cast<System::Byte>(23)),
				static_cast<System::Int32>(static_cast<System::Byte>(42)));
			this->sidebarPanel->Controls->Add(this->btnDashboard);
			this->sidebarPanel->Controls->Add(this->btnStudents);
			this->sidebarPanel->Controls->Add(this->btnTeachers);
			this->sidebarPanel->Controls->Add(this->btnClasses);
			this->sidebarPanel->Controls->Add(this->btnSubjects);
			this->sidebarPanel->Controls->Add(this->btnAcademic);
			this->sidebarPanel->Controls->Add(this->btnTimetable);
			this->sidebarPanel->Controls->Add(this->btnAttendance);
			this->sidebarPanel->Controls->Add(this->btnExaminations);
			this->sidebarPanel->Controls->Add(this->btnFees);
			this->sidebarPanel->Controls->Add(this->btnDiscipline);
			this->sidebarPanel->Controls->Add(this->btnCommunication);
			this->sidebarPanel->Controls->Add(this->btnReports);
			this->sidebarPanel->Controls->Add(this->btnUsers);
			this->sidebarPanel->Controls->Add(this->btnSettings);
			this->sidebarPanel->Dock = System::Windows::Forms::DockStyle::Left;
			this->sidebarPanel->Location = System::Drawing::Point(0, 100);
			this->sidebarPanel->Name = L"sidebarPanel";
			this->sidebarPanel->Size = System::Drawing::Size(280, 700);
			this->sidebarPanel->TabIndex = 1;
			// 
			// btnDashboard
			// 
			this->btnDashboard->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(30)), static_cast<System::Int32>(static_cast<System::Byte>(41)),
				static_cast<System::Int32>(static_cast<System::Byte>(59)));
			this->btnDashboard->FlatAppearance->BorderSize = 0;
			this->btnDashboard->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btnDashboard->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10));
			this->btnDashboard->ForeColor = System::Drawing::Color::White;
			this->btnDashboard->Location = System::Drawing::Point(15, 20);
			this->btnDashboard->Name = L"btnDashboard";
			this->btnDashboard->Size = System::Drawing::Size(200, 45);
			this->btnDashboard->TabIndex = 0;
			this->btnDashboard->Text = L"Dashboard";
			this->btnDashboard->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->btnDashboard->UseVisualStyleBackColor = false;
			// 
			// btnStudents
			// 
			this->btnStudents->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(15)), static_cast<System::Int32>(static_cast<System::Byte>(23)),
				static_cast<System::Int32>(static_cast<System::Byte>(42)));
			this->btnStudents->FlatAppearance->BorderSize = 0;
			this->btnStudents->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btnStudents->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10));
			this->btnStudents->ForeColor = System::Drawing::Color::White;
			this->btnStudents->Location = System::Drawing::Point(15, 75);
			this->btnStudents->Name = L"btnStudents";
			this->btnStudents->Size = System::Drawing::Size(200, 45);
			this->btnStudents->TabIndex = 1;
			this->btnStudents->Text = L"Students";
			this->btnStudents->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->btnStudents->UseVisualStyleBackColor = false;
			this->btnStudents->Click += gcnew System::EventHandler(this, &Dashboard::btnStudents_Click);
			// 
			// btnTeachers
			// 
			this->btnTeachers->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(15)), static_cast<System::Int32>(static_cast<System::Byte>(23)),
				static_cast<System::Int32>(static_cast<System::Byte>(42)));
			this->btnTeachers->FlatAppearance->BorderSize = 0;
			this->btnTeachers->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btnTeachers->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10));
			this->btnTeachers->ForeColor = System::Drawing::Color::White;
			this->btnTeachers->Location = System::Drawing::Point(15, 130);
			this->btnTeachers->Name = L"btnTeachers";
			this->btnTeachers->Size = System::Drawing::Size(200, 45);
			this->btnTeachers->TabIndex = 2;
			this->btnTeachers->Text = L"Teachers";
			this->btnTeachers->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->btnTeachers->UseVisualStyleBackColor = false;
			// 
			// btnClasses
			// 
			this->btnClasses->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(15)), static_cast<System::Int32>(static_cast<System::Byte>(23)),
				static_cast<System::Int32>(static_cast<System::Byte>(42)));
			this->btnClasses->FlatAppearance->BorderSize = 0;
			this->btnClasses->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btnClasses->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10));
			this->btnClasses->ForeColor = System::Drawing::Color::White;
			this->btnClasses->Location = System::Drawing::Point(15, 185);
			this->btnClasses->Name = L"btnClasses";
			this->btnClasses->Size = System::Drawing::Size(200, 45);
			this->btnClasses->TabIndex = 3;
			this->btnClasses->Text = L"Classes && Streams";
			this->btnClasses->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->btnClasses->UseVisualStyleBackColor = false;
			this->btnClasses->Click +=
				gcnew System::EventHandler(
					this,
					&Dashboard::btnClasses_Click
				);

			// 
			// btnSubjects
			// 
			this->btnSubjects->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(15)), static_cast<System::Int32>(static_cast<System::Byte>(23)),
				static_cast<System::Int32>(static_cast<System::Byte>(42)));
			this->btnSubjects->FlatAppearance->BorderSize = 0;
			this->btnSubjects->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btnSubjects->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10));
			this->btnSubjects->ForeColor = System::Drawing::Color::White;
			this->btnSubjects->Location = System::Drawing::Point(15, 240);
			this->btnSubjects->Name = L"btnSubjects";
			this->btnSubjects->Size = System::Drawing::Size(200, 45);
			this->btnSubjects->TabIndex = 4;
			this->btnSubjects->Text = L"Subjects";
			this->btnSubjects->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->btnSubjects->UseVisualStyleBackColor = false;
			// 
			// btnAcademic
			// 
			this->btnAcademic->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(15)), static_cast<System::Int32>(static_cast<System::Byte>(23)),
				static_cast<System::Int32>(static_cast<System::Byte>(42)));
			this->btnAcademic->FlatAppearance->BorderSize = 0;
			this->btnAcademic->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btnAcademic->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10));
			this->btnAcademic->ForeColor = System::Drawing::Color::White;
			this->btnAcademic->Location = System::Drawing::Point(15, 295);
			this->btnAcademic->Name = L"btnAcademic";
			this->btnAcademic->Size = System::Drawing::Size(250, 45);
			this->btnAcademic->TabIndex = 5;
			this->btnAcademic->Text = L"Academic Years && Terms";
			this->btnAcademic->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->btnAcademic->UseVisualStyleBackColor = false;

			this->btnAcademic->Click +=
				gcnew System::EventHandler(
					this,
					&Dashboard::btnAcademic_Click
				);

			// 
			// btnTimetable
			// 
			this->btnTimetable->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(15)), static_cast<System::Int32>(static_cast<System::Byte>(23)),
				static_cast<System::Int32>(static_cast<System::Byte>(42)));
			this->btnTimetable->FlatAppearance->BorderSize = 0;
			this->btnTimetable->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btnTimetable->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10));
			this->btnTimetable->ForeColor = System::Drawing::Color::White;
			this->btnTimetable->Location = System::Drawing::Point(15, 350);
			this->btnTimetable->Name = L"btnTimetable";
			this->btnTimetable->Size = System::Drawing::Size(250, 45);
			this->btnTimetable->TabIndex = 6;
			this->btnTimetable->Text = L"Timetable";
			this->btnTimetable->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->btnTimetable->UseVisualStyleBackColor = false;
			// 
			// btnAttendance
			// 
			this->btnAttendance->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(15)), static_cast<System::Int32>(static_cast<System::Byte>(23)),
				static_cast<System::Int32>(static_cast<System::Byte>(42)));
			this->btnAttendance->FlatAppearance->BorderSize = 0;
			this->btnAttendance->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btnAttendance->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10));
			this->btnAttendance->ForeColor = System::Drawing::Color::White;
			this->btnAttendance->Location = System::Drawing::Point(15, 405);
			this->btnAttendance->Name = L"btnAttendance";
			this->btnAttendance->Size = System::Drawing::Size(250, 45);
			this->btnAttendance->TabIndex = 7;
			this->btnAttendance->Text = L"Attendance";
			this->btnAttendance->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->btnAttendance->UseVisualStyleBackColor = false;
			// 
			// btnExaminations
			// 
			this->btnExaminations->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(15)), static_cast<System::Int32>(static_cast<System::Byte>(23)),
				static_cast<System::Int32>(static_cast<System::Byte>(42)));
			this->btnExaminations->FlatAppearance->BorderSize = 0;
			this->btnExaminations->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btnExaminations->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10));
			this->btnExaminations->ForeColor = System::Drawing::Color::White;
			this->btnExaminations->Location = System::Drawing::Point(15, 460);
			this->btnExaminations->Name = L"btnExaminations";
			this->btnExaminations->Size = System::Drawing::Size(250, 45);
			this->btnExaminations->TabIndex = 8;
			this->btnExaminations->Text = L"Examinations && Results";
			this->btnExaminations->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->btnExaminations->UseVisualStyleBackColor = false;
			// 
			// btnFees
			// 
			this->btnFees->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(15)), static_cast<System::Int32>(static_cast<System::Byte>(23)),
				static_cast<System::Int32>(static_cast<System::Byte>(42)));
			this->btnFees->FlatAppearance->BorderSize = 0;
			this->btnFees->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btnFees->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10));
			this->btnFees->ForeColor = System::Drawing::Color::White;
			this->btnFees->Location = System::Drawing::Point(15, 515);
			this->btnFees->Name = L"btnFees";
			this->btnFees->Size = System::Drawing::Size(250, 45);
			this->btnFees->TabIndex = 9;
			this->btnFees->Text = L"Fees && Finance";
			this->btnFees->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->btnFees->UseVisualStyleBackColor = false;
			// 
			// btnDiscipline
			// 
			this->btnDiscipline->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(15)), static_cast<System::Int32>(static_cast<System::Byte>(23)),
				static_cast<System::Int32>(static_cast<System::Byte>(42)));
			this->btnDiscipline->FlatAppearance->BorderSize = 0;
			this->btnDiscipline->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btnDiscipline->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10));
			this->btnDiscipline->ForeColor = System::Drawing::Color::White;
			this->btnDiscipline->Location = System::Drawing::Point(15, 570);
			this->btnDiscipline->Name = L"btnDiscipline";
			this->btnDiscipline->Size = System::Drawing::Size(250, 45);
			this->btnDiscipline->TabIndex = 10;
			this->btnDiscipline->Text = L"Discipline";
			this->btnDiscipline->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->btnDiscipline->UseVisualStyleBackColor = false;
			// 
			// btnCommunication
			// 
			this->btnCommunication->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(15)), static_cast<System::Int32>(static_cast<System::Byte>(23)),
				static_cast<System::Int32>(static_cast<System::Byte>(42)));
			this->btnCommunication->FlatAppearance->BorderSize = 0;
			this->btnCommunication->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btnCommunication->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10));
			this->btnCommunication->ForeColor = System::Drawing::Color::White;
			this->btnCommunication->Location = System::Drawing::Point(15, 625);
			this->btnCommunication->Name = L"btnCommunication";
			this->btnCommunication->Size = System::Drawing::Size(250, 45);
			this->btnCommunication->TabIndex = 11;
			this->btnCommunication->Text = L"Notices";
			this->btnCommunication->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->btnCommunication->UseVisualStyleBackColor = false;
			// 
			// btnReports
			// 
			this->btnReports->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(15)), static_cast<System::Int32>(static_cast<System::Byte>(23)),
				static_cast<System::Int32>(static_cast<System::Byte>(42)));
			this->btnReports->FlatAppearance->BorderSize = 0;
			this->btnReports->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btnReports->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10));
			this->btnReports->ForeColor = System::Drawing::Color::White;
			this->btnReports->Location = System::Drawing::Point(15, 680);
			this->btnReports->Name = L"btnReports";
			this->btnReports->Size = System::Drawing::Size(250, 45);
			this->btnReports->TabIndex = 12;
			this->btnReports->Text = L"Reports";
			this->btnReports->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->btnReports->UseVisualStyleBackColor = false;
			// 
			// btnUsers
			// 
			this->btnUsers->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(15)), static_cast<System::Int32>(static_cast<System::Byte>(23)),
				static_cast<System::Int32>(static_cast<System::Byte>(42)));
			this->btnUsers->FlatAppearance->BorderSize = 0;
			this->btnUsers->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btnUsers->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10));
			this->btnUsers->ForeColor = System::Drawing::Color::White;
			this->btnUsers->Location = System::Drawing::Point(15, 735);
			this->btnUsers->Name = L"btnUsers";
			this->btnUsers->Size = System::Drawing::Size(250, 45);
			this->btnUsers->TabIndex = 13;
			this->btnUsers->Text = L"Users && Roles";
			this->btnUsers->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->btnUsers->UseVisualStyleBackColor = false;
			// 
			// btnSettings
			// 
			this->btnSettings->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(15)), static_cast<System::Int32>(static_cast<System::Byte>(23)),
				static_cast<System::Int32>(static_cast<System::Byte>(42)));
			this->btnSettings->FlatAppearance->BorderSize = 0;
			this->btnSettings->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btnSettings->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10));
			this->btnSettings->ForeColor = System::Drawing::Color::White;
			this->btnSettings->Location = System::Drawing::Point(15, 790);
			this->btnSettings->Name = L"btnSettings";
			this->btnSettings->Size = System::Drawing::Size(250, 45);
			this->btnSettings->TabIndex = 14;
			this->btnSettings->Text = L"Settings";
			this->btnSettings->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->btnSettings->UseVisualStyleBackColor = false;
			// 
			// contentPanel
			// 
			this->contentPanel->BackColor = System::Drawing::Color::White;
			this->contentPanel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->contentPanel->Location = System::Drawing::Point(280, 100);
			this->contentPanel->Name = L"contentPanel";
			this->contentPanel->Size = System::Drawing::Size(1029, 700);
			this->contentPanel->TabIndex = 2;
			// 
			// Dashboard
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(9, 20);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1309, 800);
			this->Controls->Add(this->contentPanel);
			this->Controls->Add(this->sidebarPanel);
			this->Controls->Add(this->headerPanel);
			this->Name = L"Dashboard";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"SchoolCore";
			this->headerPanel->ResumeLayout(false);
			this->sidebarPanel->ResumeLayout(false);
			this->ResumeLayout(false);

		}

#pragma endregion
	};
}
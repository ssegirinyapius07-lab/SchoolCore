#pragma once

#include "DbConnection.h"

#include <mariadb/conncpp.hpp>
#include <memory>
#include <string>
#include <msclr/marshal_cppstd.h>

using namespace System;

namespace SchoolCore
{
    public ref class GradingEngine abstract sealed
    {
    private:

        static String^ GetString(
            sql::ResultSet* result,
            const char* column)
        {
            if (result->isNull(column))
                return L"";

            return gcnew String(
                result->getString(column).c_str()
            );
        }

    public:

        // Recalculate the complete subject result for one student.
        // A subject result is produced only after every active paper
        // assigned to that examination subject has a mark.
        static bool RecalculateSubjectResult(
            int examinationSubjectId,
            int studentId)
        {
            try
            {
                auto con =
                    DbConnection::GetConnection();

                int academicYearId = 0;
                int termId = 0;
                int classId = 0;
                int enrollmentId = 0;

                {
                    std::unique_ptr<sql::PreparedStatement> stmt(
                        con->prepareStatement(
                            "SELECT "
                            "e.academic_year_id, "
                            "e.term_id, "
                            "e.class_id "
                            "FROM examination_subjects es "
                            "INNER JOIN examinations e "
                            "ON e.examination_id = es.examination_id "
                            "WHERE es.examination_subject_id = ?"
                        )
                    );

                    stmt->setInt(
                        1,
                        examinationSubjectId
                    );

                    std::unique_ptr<sql::ResultSet> result(
                        stmt->executeQuery()
                    );

                    if (!result->next())
                        return false;

                    academicYearId =
                        result->getInt("academic_year_id");

                    termId =
                        result->getInt("term_id");

                    classId =
                        result->getInt("class_id");
                }

                {
                    std::unique_ptr<sql::PreparedStatement> stmt(
                        con->prepareStatement(
                            "SELECT enrollment_id "
                            "FROM enrollments "
                            "WHERE student_id = ? "
                            "AND academic_year_id = ? "
                            "AND term_id = ? "
                            "AND class_id = ? "
                            "AND status = 'Active' "
                            "ORDER BY enrollment_id DESC "
                            "LIMIT 1"
                        )
                    );

                    stmt->setInt(1, studentId);
                    stmt->setInt(2, academicYearId);
                    stmt->setInt(3, termId);
                    stmt->setInt(4, classId);

                    std::unique_ptr<sql::ResultSet> result(
                        stmt->executeQuery()
                    );

                    if (!result->next())
                        return false;

                    enrollmentId =
                        result->getInt("enrollment_id");
                }

                int academicLevelId = 0;

                {
                    std::unique_ptr<sql::PreparedStatement> stmt(
                        con->prepareStatement(
                            "SELECT academic_level_id "
                            "FROM classes "
                            "WHERE class_id = ?"
                        )
                    );

                    stmt->setInt(
                        1,
                        classId
                    );

                    std::unique_ptr<sql::ResultSet> result(
                        stmt->executeQuery()
                    );

                    if (!result->next())
                        return false;

                    academicLevelId =
                        result->getInt(
                            "academic_level_id"
                        );
                }

                int activePaperCount = 0;
                int scoredPaperCount = 0;
                double totalScore = 0.0;
                double totalMaxScore = 0.0;

                {
                    std::unique_ptr<sql::PreparedStatement> stmt(
                        con->prepareStatement(
                            "SELECT "
                            "COUNT(*) AS active_papers, "
                            "COALESCE(SUM(ep.max_score), 0) AS total_max_score, "
                            "COALESCE(SUM(CASE "
                            "WHEN m.mark_id IS NOT NULL THEN 1 "
                            "ELSE 0 END), 0) AS scored_papers, "
                            "COALESCE(SUM(COALESCE(m.score, 0)), 0) AS total_score "
                            "FROM examination_papers ep "
                            "LEFT JOIN marks m "
                            "ON m.examination_paper_id = ep.examination_paper_id "
                            "AND m.student_id = ? "
                            "WHERE ep.examination_subject_id = ? "
                            "AND ep.status = 'Active'"
                        )
                    );

                    stmt->setInt(1, studentId);
                    stmt->setInt(2, examinationSubjectId);

                    std::unique_ptr<sql::ResultSet> result(
                        stmt->executeQuery()
                    );

                    if (!result->next())
                        return false;

                    activePaperCount =
                        result->getInt(
                            "active_papers"
                        );

                    scoredPaperCount =
                        result->getInt(
                            "scored_papers"
                        );

                    totalScore =
                        result->getDouble(
                            "total_score"
                        );

                    totalMaxScore =
                        result->getDouble(
                            "total_max_score"
                        );
                }

                if (
                    activePaperCount == 0 ||
                    scoredPaperCount != activePaperCount ||
                    totalMaxScore <= 0.0)
                {
                    return false;
                }

                double percentage =
                    (totalScore / totalMaxScore) * 100.0;

                String^ grade = L"";
                double gradePoint = 0.0;
                bool hasGradePoint = false;
                String^ remarks = L"";

                {
                    std::unique_ptr<sql::PreparedStatement> stmt(
                        con->prepareStatement(
                            "SELECT "
                            "grade, "
                            "grade_point, "
                            "remarks "
                            "FROM grading_scales "
                            "WHERE academic_level_id = ? "
                            "AND status = 'Active' "
                            "AND min_score <= ? "
                            "AND max_score >= ? "
                            "ORDER BY min_score DESC "
                            "LIMIT 1"
                        )
                    );

                    stmt->setInt(
                        1,
                        academicLevelId
                    );

                    stmt->setDouble(
                        2,
                        percentage
                    );

                    stmt->setDouble(
                        3,
                        percentage
                    );

                    std::unique_ptr<sql::ResultSet> result(
                        stmt->executeQuery()
                    );

                    if (result->next())
                    {
                        grade =
                            GetString(
                                result.get(),
                                "grade"
                            );

                        if (!result->isNull("grade_point"))
                        {
                            gradePoint =
                                result->getDouble(
                                    "grade_point"
                                );

                            hasGradePoint = true;
                        }

                        remarks =
                            GetString(
                                result.get(),
                                "remarks"
                            );
                    }
                }

                {
                    std::unique_ptr<sql::PreparedStatement> stmt(
                        con->prepareStatement(
                            "INSERT INTO student_subject_results "
                            "(enrollment_id, examination_subject_id, "
                            "total_score, total_max_score, percentage, "
                            "grade, grade_point, remarks, paper_count, status, calculated_at) "
                            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, 'Calculated', CURRENT_TIMESTAMP) "
                            "ON DUPLICATE KEY UPDATE "
                            "total_score = VALUES(total_score), "
                            "total_max_score = VALUES(total_max_score), "
                            "percentage = VALUES(percentage), "
                            "grade = VALUES(grade), "
                            "grade_point = VALUES(grade_point), "
                            "remarks = VALUES(remarks), "
                            "paper_count = VALUES(paper_count), "
                            "status = 'Calculated', "
                            "calculated_at = CURRENT_TIMESTAMP"
                        )
                    );

                    stmt->setInt(1, enrollmentId);
                    stmt->setInt(2, examinationSubjectId);
                    stmt->setDouble(3, totalScore);
                    stmt->setDouble(4, totalMaxScore);
                    stmt->setDouble(5, percentage);

                    if (String::IsNullOrWhiteSpace(grade))
                        stmt->setNull(6, sql::DataType::VARCHAR);
                    else
                        stmt->setString(
                            6,
                            msclr::interop::marshal_as<std::string>(
                                grade
                            )
                        );

                    if (hasGradePoint)
                        stmt->setDouble(7, gradePoint);
                    else
                        stmt->setNull(7, sql::DataType::DECIMAL);

                    if (String::IsNullOrWhiteSpace(remarks))
                        stmt->setNull(8, sql::DataType::VARCHAR);
                    else
                        stmt->setString(
                            8,
                            msclr::interop::marshal_as<std::string>(
                                remarks
                            )
                        );

                    stmt->setInt(
                        9,
                        activePaperCount
                    );

                    stmt->executeUpdate();
                }

                return true;
            }
            catch (sql::SQLException&)
            {
                return false;
            }
        }
    };
}

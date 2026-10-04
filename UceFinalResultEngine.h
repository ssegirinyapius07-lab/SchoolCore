#pragma once

#include "DbConnection.h"

#include <mariadb/conncpp.hpp>
#include <memory>
#include <string>
#include <msclr/marshal_cppstd.h>

using namespace System;

namespace SchoolCore
{
    public ref class UceFinalResultEngine abstract sealed
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

        // Calculates a final UCE/CBC subject result.
        //
        // The current UCE result model combines:
        //   Continuous Assessment = 20%
        //   End-of-Cycle examination = 80%
        //
        // The examination percentage is itself calculated from all active
        // papers assigned to the examination subject using each paper's
        // actual maximum score. This preserves paper balancing without
        // grading a subject from one paper alone.
        //
        // This engine is intentionally separate from GradingEngine so
        // Mid-Term and other internal examinations remain examination-only
        // results until a final UCE result is explicitly calculated.
        static bool RecalculateFinalSubjectResult(
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
                int subjectId = 0;
                int enrollmentId = 0;
                int academicLevelId = 0;

                {
                    std::unique_ptr<sql::PreparedStatement> stmt(
                        con->prepareStatement(
                            "SELECT "
                            "e.academic_year_id, "
                            "e.term_id, "
                            "e.class_id, "
                            "es.subject_id, "
                            "c.academic_level_id "
                            "FROM examination_subjects es "
                            "INNER JOIN examinations e "
                            "ON e.examination_id = es.examination_id "
                            "INNER JOIN classes c "
                            "ON c.class_id = e.class_id "
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

                    subjectId =
                        result->getInt("subject_id");

                    academicLevelId =
                        result->getInt("academic_level_id");
                }

                // UCE/CBC final result engine is for O-Level.
                if (academicLevelId != 1)
                    return false;

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

                double caScore = 0.0;

                {
                    std::unique_ptr<sql::PreparedStatement> stmt(
                        con->prepareStatement(
                            "SELECT ca_score "
                            "FROM student_subject_continuous_assessments "
                            "WHERE enrollment_id = ? "
                            "AND subject_id = ? "
                            "AND status = 'Active' "
                            "LIMIT 1"
                        )
                    );

                    stmt->setInt(1, enrollmentId);
                    stmt->setInt(2, subjectId);

                    std::unique_ptr<sql::ResultSet> result(
                        stmt->executeQuery()
                    );

                    // A final UCE result cannot be calculated until CA exists.
                    if (!result->next())
                        return false;

                    caScore =
                        result->getDouble("ca_score");
                }

                int activePaperCount = 0;
                int scoredPaperCount = 0;
                double totalExamScore = 0.0;
                double totalExamMaxScore = 0.0;

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
                        result->getInt("active_papers");

                    scoredPaperCount =
                        result->getInt("scored_papers");

                    totalExamScore =
                        result->getDouble("total_score");

                    totalExamMaxScore =
                        result->getDouble("total_max_score");
                }

                if (
                    activePaperCount == 0 ||
                    scoredPaperCount != activePaperCount ||
                    totalExamMaxScore <= 0.0)
                {
                    return false;
                }

                // Paper-balanced examination percentage.
                double examinationPercentage =
                    (totalExamScore / totalExamMaxScore) * 100.0;

                // Official UCE/CBC weighting model: 20% CA + 80% examination.
                double finalScore =
                    (caScore * 0.20) +
                    (examinationPercentage * 0.80);

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

                    stmt->setInt(1, academicLevelId);
                    stmt->setDouble(2, finalScore);
                    stmt->setDouble(3, finalScore);

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
                                result->getDouble("grade_point");

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
                            "INSERT INTO student_subject_final_results "
                            "(enrollment_id, examination_subject_id, "
                            "ca_score, ca_weight, examination_score, examination_weight, "
                            "final_score, grade, grade_point, remarks, "
                            "calculation_method, status, calculated_at) "
                            "VALUES (?, ?, ?, 20.00, ?, 80.00, ?, ?, ?, ?, "
                            "'UCE_20_80', 'Calculated', CURRENT_TIMESTAMP) "
                            "ON DUPLICATE KEY UPDATE "
                            "ca_score = VALUES(ca_score), "
                            "ca_weight = VALUES(ca_weight), "
                            "examination_score = VALUES(examination_score), "
                            "examination_weight = VALUES(examination_weight), "
                            "final_score = VALUES(final_score), "
                            "grade = VALUES(grade), "
                            "grade_point = VALUES(grade_point), "
                            "remarks = VALUES(remarks), "
                            "calculation_method = VALUES(calculation_method), "
                            "status = 'Calculated', "
                            "calculated_at = CURRENT_TIMESTAMP"
                        )
                    );

                    stmt->setInt(1, enrollmentId);
                    stmt->setInt(2, examinationSubjectId);
                    stmt->setDouble(3, caScore);
                    stmt->setDouble(4, examinationPercentage);
                    stmt->setDouble(5, finalScore);

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

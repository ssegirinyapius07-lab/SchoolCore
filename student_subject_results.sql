-- SchoolCore: aggregate subject results from examination papers.
-- Run once in schoolcore_db.
--
-- marks stores one score per student per examination paper.
-- This table stores the calculated result for the whole subject
-- after the configured papers have been considered.

CREATE TABLE IF NOT EXISTS student_subject_results (
    student_subject_result_id INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
    enrollment_id INT(10) UNSIGNED NOT NULL,
    examination_subject_id INT(10) UNSIGNED NOT NULL,
    total_score DECIMAL(10,2) NOT NULL DEFAULT 0,
    total_max_score DECIMAL(10,2) NOT NULL DEFAULT 0,
    percentage DECIMAL(6,2) NOT NULL DEFAULT 0,
    grade VARCHAR(20) NULL,
    grade_point DECIMAL(6,2) NULL,
    remarks VARCHAR(255) NULL,
    paper_count INT(10) UNSIGNED NOT NULL DEFAULT 0,
    status VARCHAR(30) NOT NULL DEFAULT 'Calculated',
    calculated_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (student_subject_result_id),
    UNIQUE KEY uq_student_subject_result
        (enrollment_id, examination_subject_id),
    KEY idx_student_subject_results_enrollment
        (enrollment_id),
    KEY idx_student_subject_results_examination_subject
        (examination_subject_id),
    CONSTRAINT fk_ssr_enrollment
        FOREIGN KEY (enrollment_id)
        REFERENCES enrollments (enrollment_id)
        ON UPDATE CASCADE
        ON DELETE CASCADE,
    CONSTRAINT fk_ssr_examination_subject
        FOREIGN KEY (examination_subject_id)
        REFERENCES examination_subjects (examination_subject_id)
        ON UPDATE CASCADE
        ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

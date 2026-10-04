-- SchoolCore: UCE final-results structure.
-- The current UCE/CBC result model combines Continuous Assessment (20%)
-- with the End-of-Cycle examination (80%).
--
-- This table stores the school-entered subject CA score on a 0-100 basis.
-- The final-result engine applies the 20/80 weighting.
--
-- The existing student_subject_results table remains the examination-result
-- table. Mid-term and other internal examinations continue to use it without
-- being treated as a final UCE result.

CREATE TABLE IF NOT EXISTS student_subject_continuous_assessments (
    continuous_assessment_id INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
    enrollment_id INT(10) UNSIGNED NOT NULL,
    subject_id INT(10) UNSIGNED NOT NULL,
    ca_score DECIMAL(6,2) NOT NULL,
    status VARCHAR(30) NOT NULL DEFAULT 'Active',
    entered_by INT(10) UNSIGNED DEFAULT NULL,
    entered_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP
        ON UPDATE CURRENT_TIMESTAMP,
    PRIMARY KEY (continuous_assessment_id),
    UNIQUE KEY uq_student_subject_ca
        (enrollment_id, subject_id),
    KEY idx_student_subject_ca_enrollment
        (enrollment_id),
    KEY idx_student_subject_ca_subject
        (subject_id),
    CONSTRAINT fk_student_subject_ca_enrollment
        FOREIGN KEY (enrollment_id)
        REFERENCES enrollments (enrollment_id)
        ON DELETE CASCADE
        ON UPDATE CASCADE,
    CONSTRAINT fk_student_subject_ca_subject
        FOREIGN KEY (subject_id)
        REFERENCES subjects (subject_id)
        ON DELETE RESTRICT
        ON UPDATE CASCADE,
    CONSTRAINT fk_student_subject_ca_user
        FOREIGN KEY (entered_by)
        REFERENCES users (user_id)
        ON DELETE SET NULL
        ON UPDATE CASCADE
);

CREATE TABLE IF NOT EXISTS student_subject_final_results (
    student_subject_final_result_id INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
    enrollment_id INT(10) UNSIGNED NOT NULL,
    examination_subject_id INT(10) UNSIGNED NOT NULL,
    ca_score DECIMAL(6,2) NOT NULL,
    ca_weight DECIMAL(6,2) NOT NULL DEFAULT 20.00,
    examination_score DECIMAL(6,2) NOT NULL,
    examination_weight DECIMAL(6,2) NOT NULL DEFAULT 80.00,
    final_score DECIMAL(6,2) NOT NULL,
    grade VARCHAR(20) DEFAULT NULL,
    grade_point DECIMAL(6,2) DEFAULT NULL,
    remarks VARCHAR(255) DEFAULT NULL,
    calculation_method VARCHAR(50) NOT NULL DEFAULT 'UCE_20_80',
    status VARCHAR(30) NOT NULL DEFAULT 'Calculated',
    calculated_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (student_subject_final_result_id),
    UNIQUE KEY uq_student_subject_final_result
        (enrollment_id, examination_subject_id),
    KEY idx_student_subject_final_enrollment
        (enrollment_id),
    KEY idx_student_subject_final_exam_subject
        (examination_subject_id),
    CONSTRAINT fk_student_subject_final_enrollment
        FOREIGN KEY (enrollment_id)
        REFERENCES enrollments (enrollment_id)
        ON DELETE CASCADE
        ON UPDATE CASCADE,
    CONSTRAINT fk_student_subject_final_exam_subject
        FOREIGN KEY (examination_subject_id)
        REFERENCES examination_subjects (examination_subject_id)
        ON DELETE CASCADE
        ON UPDATE CASCADE
);

-- Defensive validation for the CA score.
DROP TRIGGER IF EXISTS trg_student_subject_ca_validate_insert;
DELIMITER $$
CREATE TRIGGER trg_student_subject_ca_validate_insert
BEFORE INSERT ON student_subject_continuous_assessments
FOR EACH ROW
BEGIN
    IF NEW.ca_score < 0 OR NEW.ca_score > 100 THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'Continuous Assessment score must be between 0 and 100.';
    END IF;
END
$$
DELIMITER ;

DROP TRIGGER IF EXISTS trg_student_subject_ca_validate_update;
DELIMITER $$
CREATE TRIGGER trg_student_subject_ca_validate_update
BEFORE UPDATE ON student_subject_continuous_assessments
FOR EACH ROW
BEGIN
    IF NEW.ca_score < 0 OR NEW.ca_score > 100 THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'Continuous Assessment score must be between 0 and 100.';
    END IF;
END
$$
DELIMITER ;

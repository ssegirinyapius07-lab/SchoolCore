-- SchoolCore: student-specific O-Level optional subject selections
-- Run once in the schoolcore_db database.
-- A student's S3 enrollment can carry exactly two optional subject selections.
-- When the student is promoted, the selections can be copied to the new enrollment
-- so the student's chosen options remain part of the academic history.

CREATE TABLE IF NOT EXISTS enrollment_optional_subjects (
    enrollment_optional_subject_id INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
    enrollment_id INT(10) UNSIGNED NOT NULL,
    subject_id INT(10) UNSIGNED NOT NULL,
    option_number TINYINT UNSIGNED NOT NULL,
    selected_date DATE NULL,
    status VARCHAR(30) NOT NULL DEFAULT 'Active',
    PRIMARY KEY (enrollment_optional_subject_id),
    UNIQUE KEY uq_enrollment_optional_subject (
        enrollment_id,
        subject_id
    ),
    UNIQUE KEY uq_enrollment_optional_number (
        enrollment_id,
        option_number
    ),
    KEY fk_enrollment_optional_subjects_subject (
        subject_id
    ),
    CONSTRAINT chk_enrollment_optional_number
        CHECK (option_number IN (1, 2)),
    CONSTRAINT fk_enrollment_optional_subjects_enrollment
        FOREIGN KEY (enrollment_id)
        REFERENCES enrollments (enrollment_id)
        ON UPDATE CASCADE
        ON DELETE CASCADE,
    CONSTRAINT fk_enrollment_optional_subjects_subject
        FOREIGN KEY (subject_id)
        REFERENCES subjects (subject_id)
        ON UPDATE CASCADE
        ON DELETE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

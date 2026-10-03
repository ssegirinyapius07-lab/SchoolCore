-- SchoolCore academic and examination structure
-- Run once in the schoolcore_db database.
-- Existing student, class, stream, examination, and subject data are preserved.

CREATE TABLE IF NOT EXISTS academic_levels (
    academic_level_id INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
    level_code VARCHAR(30) NOT NULL,
    level_name VARCHAR(100) NOT NULL,
    description VARCHAR(255) NULL,
    status VARCHAR(30) NOT NULL DEFAULT 'Active',
    PRIMARY KEY (academic_level_id),
    UNIQUE KEY uq_academic_levels_code (level_code),
    UNIQUE KEY uq_academic_levels_name (level_name)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

INSERT INTO academic_levels (
    level_code,
    level_name,
    description,
    status
)
VALUES
    ('O_LEVEL', 'O-Level', 'Lower secondary academic level, Senior 1 to Senior 4.', 'Active'),
    ('A_LEVEL', 'A-Level', 'Advanced secondary academic level, Senior 5 to Senior 6.', 'Active')
ON DUPLICATE KEY UPDATE
    level_name = VALUES(level_name),
    description = VALUES(description),
    status = VALUES(status);


-- Link the existing Senior 1-6 classes to an explicit academic level.
ALTER TABLE classes
    ADD COLUMN academic_level_id INT(10) UNSIGNED NULL AFTER class_name;

UPDATE classes
SET academic_level_id =
    (
        SELECT academic_level_id
        FROM academic_levels
        WHERE level_code = 'O_LEVEL'
        LIMIT 1
    )
WHERE class_name IN ('Senior 1', 'Senior 2', 'Senior 3', 'Senior 4');

UPDATE classes
SET academic_level_id =
    (
        SELECT academic_level_id
        FROM academic_levels
        WHERE level_code = 'A_LEVEL'
        LIMIT 1
    )
WHERE class_name IN ('Senior 5', 'Senior 6');

ALTER TABLE classes
    MODIFY COLUMN academic_level_id INT(10) UNSIGNED NOT NULL;

ALTER TABLE classes
    ADD CONSTRAINT fk_classes_academic_level
    FOREIGN KEY (academic_level_id)
    REFERENCES academic_levels (academic_level_id)
    ON UPDATE CASCADE
    ON DELETE RESTRICT;


-- A-Level combinations such as BCM, PCB, BCM/ICT, HEG, etc.
CREATE TABLE IF NOT EXISTS subject_combinations (
    combination_id INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
    academic_level_id INT(10) UNSIGNED NOT NULL,
    combination_code VARCHAR(50) NOT NULL,
    combination_name VARCHAR(100) NOT NULL,
    description VARCHAR(255) NULL,
    status VARCHAR(30) NOT NULL DEFAULT 'Active',
    PRIMARY KEY (combination_id),
    UNIQUE KEY uq_subject_combinations_code (combination_code),
    CONSTRAINT fk_subject_combinations_level
        FOREIGN KEY (academic_level_id)
        REFERENCES academic_levels (academic_level_id)
        ON UPDATE CASCADE
        ON DELETE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;


-- Subjects belonging to each combination.
-- role: Principal, Subsidiary, or General.
CREATE TABLE IF NOT EXISTS combination_subjects (
    combination_subject_id INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
    combination_id INT(10) UNSIGNED NOT NULL,
    subject_id INT(10) UNSIGNED NOT NULL,
    subject_role VARCHAR(30) NOT NULL DEFAULT 'Principal',
    status VARCHAR(30) NOT NULL DEFAULT 'Active',
    PRIMARY KEY (combination_subject_id),
    UNIQUE KEY uq_combination_subject (combination_id, subject_id),
    CONSTRAINT fk_combination_subjects_combination
        FOREIGN KEY (combination_id)
        REFERENCES subject_combinations (combination_id)
        ON UPDATE CASCADE
        ON DELETE CASCADE,
    CONSTRAINT fk_combination_subjects_subject
        FOREIGN KEY (subject_id)
        REFERENCES subjects (subject_id)
        ON UPDATE CASCADE
        ON DELETE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;


-- Allow an A-Level enrollment to carry its combination.
ALTER TABLE enrollments
    ADD COLUMN combination_id INT(10) UNSIGNED NULL AFTER stream_id;

ALTER TABLE enrollments
    ADD CONSTRAINT fk_enrollments_combination
    FOREIGN KEY (combination_id)
    REFERENCES subject_combinations (combination_id)
    ON UPDATE CASCADE
    ON DELETE RESTRICT;


-- Grading scales are configurable by academic level.
ALTER TABLE grading_scales
    ADD COLUMN academic_level_id INT(10) UNSIGNED NULL AFTER grading_scale_id;

ALTER TABLE grading_scales
    ADD CONSTRAINT fk_grading_scales_level
    FOREIGN KEY (academic_level_id)
    REFERENCES academic_levels (academic_level_id)
    ON UPDATE CASCADE
    ON DELETE RESTRICT;

ALTER TABLE grading_scales
    ADD UNIQUE KEY uq_grading_scale_level_name
    (academic_level_id, scale_name);


-- Prevent the same subject from being attached twice to one examination.
ALTER TABLE examination_subjects
    ADD UNIQUE KEY uq_examination_subject
    (examination_id, subject_id);


-- Prevent duplicate marks for the same student in the same examination subject.
ALTER TABLE marks
    ADD UNIQUE KEY uq_marks_examination_subject_student
    (examination_subject_id, student_id);

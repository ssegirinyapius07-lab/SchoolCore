-- SchoolCore: curriculum-aware UNEB subject and paper setup
-- Run once in schoolcore_db.
-- This keeps general subjects in subjects and stores UNEB identification
-- against a curriculum-specific subject record.

CREATE TABLE IF NOT EXISTS curricula (
    curriculum_id INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
    curriculum_code VARCHAR(50) NOT NULL,
    curriculum_name VARCHAR(150) NOT NULL,
    academic_level_id INT(10) UNSIGNED NOT NULL,
    effective_from_year INT NULL,
    effective_to_year INT NULL,
    description VARCHAR(255) NULL,
    status VARCHAR(30) NOT NULL DEFAULT 'Active',
    PRIMARY KEY (curriculum_id),
    UNIQUE KEY uq_curricula_code (curriculum_code),
    UNIQUE KEY uq_curricula_name (curriculum_name),
    CONSTRAINT fk_curricula_academic_level
        FOREIGN KEY (academic_level_id)
        REFERENCES academic_levels (academic_level_id)
        ON UPDATE CASCADE
        ON DELETE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

INSERT INTO curricula (
    curriculum_code,
    curriculum_name,
    academic_level_id,
    description,
    status
)
SELECT
    'UCE_CBC',
    'UCE Competency Based Curriculum',
    al.academic_level_id,
    'Current UCE curriculum used for the new lower secondary assessment structure.',
    'Active'
FROM academic_levels al
WHERE al.level_code = 'O_LEVEL'
  AND NOT EXISTS (
      SELECT 1
      FROM curricula c
      WHERE c.curriculum_code = 'UCE_CBC'
  );

INSERT INTO curricula (
    curriculum_code,
    curriculum_name,
    academic_level_id,
    description,
    status
)
SELECT
    'UACE_ALIGNED',
    'UACE Aligned Curriculum',
    al.academic_level_id,
    'Current UACE aligned curriculum assessment structure.',
    'Active'
FROM academic_levels al
WHERE al.level_code = 'A_LEVEL'
  AND NOT EXISTS (
      SELECT 1
      FROM curricula c
      WHERE c.curriculum_code = 'UACE_ALIGNED'
  );


CREATE TABLE IF NOT EXISTS curriculum_subjects (
    curriculum_subject_id INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
    curriculum_id INT(10) UNSIGNED NOT NULL,
    subject_id INT(10) UNSIGNED NOT NULL,
    uneb_subject_code VARCHAR(30) NOT NULL,
    subject_group VARCHAR(60) NOT NULL DEFAULT 'Other',
    status VARCHAR(30) NOT NULL DEFAULT 'Active',
    PRIMARY KEY (curriculum_subject_id),
    UNIQUE KEY uq_curriculum_subject (curriculum_id, subject_id),
    UNIQUE KEY uq_curriculum_subject_code (curriculum_id, uneb_subject_code),
    CONSTRAINT fk_curriculum_subjects_curriculum
        FOREIGN KEY (curriculum_id)
        REFERENCES curricula (curriculum_id)
        ON UPDATE CASCADE
        ON DELETE RESTRICT,
    CONSTRAINT fk_curriculum_subjects_subject
        FOREIGN KEY (subject_id)
        REFERENCES subjects (subject_id)
        ON UPDATE CASCADE
        ON DELETE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;


-- Academic years can identify the curriculum in force for that year.
ALTER TABLE academic_years
    ADD COLUMN curriculum_id INT(10) UNSIGNED NULL AFTER year_name;

ALTER TABLE academic_years
    ADD CONSTRAINT fk_academic_years_curriculum
    FOREIGN KEY (curriculum_id)
    REFERENCES curricula (curriculum_id)
    ON UPDATE CASCADE
    ON DELETE RESTRICT;


-- Paper master data is attached to the curriculum-specific subject record.
ALTER TABLE subject_papers
    ADD COLUMN curriculum_subject_id INT(10) UNSIGNED NULL AFTER paper_id;

ALTER TABLE subject_papers
    MODIFY COLUMN subject_id INT(10) UNSIGNED NULL,
    MODIFY COLUMN academic_level_id INT(10) UNSIGNED NULL;

-- Keep a dedicated index for the existing subject foreign key.
-- The old composite unique index is also being replaced below.
ALTER TABLE subject_papers
    ADD INDEX idx_subject_papers_subject_id (subject_id);

ALTER TABLE subject_papers
    DROP INDEX uq_subject_paper_code;

ALTER TABLE subject_papers
    ADD UNIQUE KEY uq_curriculum_subject_paper_code
    (curriculum_subject_id, paper_code);

ALTER TABLE subject_papers
    ADD CONSTRAINT fk_subject_papers_curriculum_subject
    FOREIGN KEY (curriculum_subject_id)
    REFERENCES curriculum_subjects (curriculum_subject_id)
    ON UPDATE CASCADE
    ON DELETE RESTRICT;


-- Seed the paper table from the curriculum subject record going forward.
-- No paper codes are invented here; administrators enter official UNEB codes.

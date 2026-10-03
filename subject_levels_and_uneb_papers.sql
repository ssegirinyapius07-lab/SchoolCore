-- SchoolCore: subject academic-level assignment and standard UNEB paper master data
-- Run once in the schoolcore_db database.
-- Existing subjects remain in the general subjects table.

CREATE TABLE IF NOT EXISTS subject_academic_levels (
    subject_academic_level_id INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
    subject_id INT(10) UNSIGNED NOT NULL,
    academic_level_id INT(10) UNSIGNED NOT NULL,
    status VARCHAR(30) NOT NULL DEFAULT 'Active',
    PRIMARY KEY (subject_academic_level_id),
    UNIQUE KEY uq_subject_academic_level (subject_id, academic_level_id),
    CONSTRAINT fk_subject_academic_levels_subject
        FOREIGN KEY (subject_id)
        REFERENCES subjects (subject_id)
        ON UPDATE CASCADE
        ON DELETE CASCADE,
    CONSTRAINT fk_subject_academic_levels_level
        FOREIGN KEY (academic_level_id)
        REFERENCES academic_levels (academic_level_id)
        ON UPDATE CASCADE
        ON DELETE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;


-- Master list of standard examination papers.
-- paper_code stores the official code such as 112/1, 456/1, etc.
-- Do not generate these codes automatically.
CREATE TABLE IF NOT EXISTS subject_papers (
    paper_id INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
    subject_id INT(10) UNSIGNED NOT NULL,
    academic_level_id INT(10) UNSIGNED NOT NULL,
    paper_code VARCHAR(30) NOT NULL,
    paper_number VARCHAR(20) NULL,
    paper_name VARCHAR(150) NULL,
    paper_type VARCHAR(30) NOT NULL DEFAULT 'Theory',
    status VARCHAR(30) NOT NULL DEFAULT 'Active',
    PRIMARY KEY (paper_id),
    UNIQUE KEY uq_subject_paper_code (subject_id, academic_level_id, paper_code),
    CONSTRAINT fk_subject_papers_subject
        FOREIGN KEY (subject_id)
        REFERENCES subjects (subject_id)
        ON UPDATE CASCADE
        ON DELETE RESTRICT,
    CONSTRAINT fk_subject_papers_level
        FOREIGN KEY (academic_level_id)
        REFERENCES academic_levels (academic_level_id)
        ON UPDATE CASCADE
        ON DELETE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;


-- Papers selected for a particular examination.
-- This allows one subject to have more than one paper.
CREATE TABLE IF NOT EXISTS examination_papers (
    examination_paper_id INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
    examination_subject_id INT(10) UNSIGNED NOT NULL,
    paper_id INT(10) UNSIGNED NOT NULL,
    max_score DECIMAL(10,2) NOT NULL DEFAULT 100,
    pass_mark DECIMAL(10,2) NULL,
    status VARCHAR(30) NOT NULL DEFAULT 'Active',
    PRIMARY KEY (examination_paper_id),
    UNIQUE KEY uq_examination_subject_paper
        (examination_subject_id, paper_id),
    CONSTRAINT fk_examination_papers_subject
        FOREIGN KEY (examination_subject_id)
        REFERENCES examination_subjects (examination_subject_id)
        ON UPDATE CASCADE
        ON DELETE CASCADE,
    CONSTRAINT fk_examination_papers_paper
        FOREIGN KEY (paper_id)
        REFERENCES subject_papers (paper_id)
        ON UPDATE CASCADE
        ON DELETE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

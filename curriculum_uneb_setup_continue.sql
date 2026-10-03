-- SchoolCore: continue curriculum-aware UNEB setup after the
-- uq_subject_paper_code migration error.
-- Use this ONLY after the original migration stopped at:
-- DROP INDEX uq_subject_paper_code
--
-- The earlier statements in curriculum_uneb_setup.sql have already run.

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

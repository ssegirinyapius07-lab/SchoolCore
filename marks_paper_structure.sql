-- SchoolCore: link marks to the actual examination paper.
-- Run once in schoolcore_db.
--
-- The examination already has examination_papers records such as
-- Paper 1 / Paper 2 with official paper_code, paper_name, max_score
-- and pass_mark. Marks must be stored against that paper so that
-- the same student can have separate marks for multiple papers
-- belonging to the same subject.

ALTER TABLE marks
    ADD COLUMN examination_paper_id INT(10) UNSIGNED NULL
    AFTER examination_subject_id;

-- The previous subject-level uniqueness prevented Paper 1 and Paper 2
-- from being stored separately for the same student.
ALTER TABLE marks
    DROP INDEX examination_subject_id,
    DROP INDEX uq_marks_examination_subject_student;

ALTER TABLE marks
    ADD KEY idx_marks_examination_paper_student
    (examination_paper_id, student_id);

ALTER TABLE marks
    ADD CONSTRAINT fk_marks_examination_paper
    FOREIGN KEY (examination_paper_id)
    REFERENCES examination_papers (examination_paper_id)
    ON DELETE CASCADE
    ON UPDATE CASCADE;

-- A student can have one mark entry per examination paper.
ALTER TABLE marks
    ADD UNIQUE KEY uq_marks_examination_paper_student
    (examination_paper_id, student_id);

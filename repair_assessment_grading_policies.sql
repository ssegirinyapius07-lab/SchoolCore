-- Repair the partially applied assessment_grading_policies.sql migration.
-- This script is safe to run after the duplicate P425/1 error.
-- It preserves existing examination_papers and their marks by reusing
-- existing paper rows instead of deleting referenced rows.

START TRANSACTION;

SET @uace_curriculum_id := (
    SELECT curriculum_id
    FROM curricula
    WHERE curriculum_code = 'UACE_ALIGNED'
    LIMIT 1
);

SET @math_cs_id := (
    SELECT MIN(curriculum_subject_id)
    FROM curriculum_subjects
    WHERE curriculum_id = @uace_curriculum_id
      AND subject_id = 1
      AND status = 'Active'
);

SET @legacy_p1_id := (
    SELECT paper_id
    FROM subject_papers
    WHERE curriculum_subject_id = @math_cs_id
      AND paper_code = 'P425'
    ORDER BY paper_id ASC
    LIMIT 1
);

SET @canonical_p1_id := (
    SELECT paper_id
    FROM subject_papers
    WHERE curriculum_subject_id = @math_cs_id
      AND paper_code = 'P425/1'
    ORDER BY paper_id ASC
    LIMIT 1
);

SET @legacy_p2_id := (
    SELECT paper_id
    FROM subject_papers
    WHERE curriculum_subject_id = @math_cs_id
      AND paper_code = '245'
    ORDER BY paper_id ASC
    LIMIT 1
);

SET @canonical_p2_id := (
    SELECT paper_id
    FROM subject_papers
    WHERE curriculum_subject_id = @math_cs_id
      AND paper_code = 'P425/2'
    ORDER BY paper_id ASC
    LIMIT 1
);

-- 1. Preserve the original P425 paper row because the user's existing
--    examination_papers/marks may point to it. Temporarily free its code.
UPDATE subject_papers
SET paper_code = CONCAT('TMP-P425-', paper_id)
WHERE paper_id = @legacy_p1_id
  AND @legacy_p1_id IS NOT NULL
  AND @canonical_p1_id IS NOT NULL
  AND @legacy_p1_id <> @canonical_p1_id;

-- 2. Repoint references from the duplicate newly-created P425/1 row
--    to the original paper row, when the target does not already exist
--    for the same examination subject.
UPDATE examination_papers ep
SET ep.paper_id = @legacy_p1_id
WHERE ep.paper_id = @canonical_p1_id
  AND @canonical_p1_id IS NOT NULL
  AND @legacy_p1_id IS NOT NULL
  AND NOT EXISTS (
      SELECT 1
      FROM examination_papers conflict_ep
      WHERE conflict_ep.examination_subject_id = ep.examination_subject_id
        AND conflict_ep.paper_id = @legacy_p1_id
        AND conflict_ep.examination_paper_id <> ep.examination_paper_id
  );

-- 3. The old P425 row is now the canonical P425/1 row.
UPDATE subject_papers
SET
    paper_code = 'P425/1',
    paper_number = '1',
    paper_name = 'Principal Mathematics Paper 1',
    paper_type = 'Theory',
    status = 'Active'
WHERE paper_id = @legacy_p1_id
  AND @legacy_p1_id IS NOT NULL;

-- 4. The duplicate row, if still present, is made inactive so the grading
--    engine cannot count it as another required paper.
UPDATE subject_papers
SET
    paper_code = CONCAT('DUP-P425-', paper_id),
    status = 'Inactive'
WHERE paper_id = @canonical_p1_id
  AND @canonical_p1_id IS NOT NULL
  AND @canonical_p1_id <> @legacy_p1_id;

-- 5. Do the same normalization for the old Biology-coded Mathematics row 245.
UPDATE subject_papers
SET paper_code = CONCAT('TMP-245-', paper_id)
WHERE paper_id = @legacy_p2_id
  AND @legacy_p2_id IS NOT NULL
  AND @canonical_p2_id IS NOT NULL
  AND @legacy_p2_id <> @canonical_p2_id;

UPDATE examination_papers ep
SET ep.paper_id = @legacy_p2_id
WHERE ep.paper_id = @canonical_p2_id
  AND @canonical_p2_id IS NOT NULL
  AND @legacy_p2_id IS NOT NULL
  AND NOT EXISTS (
      SELECT 1
      FROM examination_papers conflict_ep
      WHERE conflict_ep.examination_subject_id = ep.examination_subject_id
        AND conflict_ep.paper_id = @legacy_p2_id
        AND conflict_ep.examination_paper_id <> ep.examination_paper_id
  );

UPDATE subject_papers
SET
    paper_code = 'P425/2',
    paper_number = '2',
    paper_name = 'Principal Mathematics Paper 2',
    paper_type = 'Theory',
    status = 'Active'
WHERE paper_id = @legacy_p2_id
  AND @legacy_p2_id IS NOT NULL;

UPDATE subject_papers
SET
    paper_code = CONCAT('DUP-245-', paper_id),
    status = 'Inactive'
WHERE paper_id = @canonical_p2_id
  AND @canonical_p2_id IS NOT NULL
  AND @canonical_p2_id <> @legacy_p2_id;

-- 6. Ensure the two Mathematics papers exist even when one of the original
--    malformed rows was absent.
INSERT INTO subject_papers
    (curriculum_subject_id, subject_id, academic_level_id,
     paper_code, paper_number, paper_name, paper_type, status)
SELECT
    @math_cs_id, 1, 2, 'P425/1', '1',
    'Principal Mathematics Paper 1', 'Theory', 'Active'
WHERE @math_cs_id IS NOT NULL
  AND NOT EXISTS (
      SELECT 1
      FROM subject_papers
      WHERE curriculum_subject_id = @math_cs_id
        AND paper_code = 'P425/1'
        AND status = 'Active'
  );

INSERT INTO subject_papers
    (curriculum_subject_id, subject_id, academic_level_id,
     paper_code, paper_number, paper_name, paper_type, status)
SELECT
    @math_cs_id, 1, 2, 'P425/2', '2',
    'Principal Mathematics Paper 2', 'Theory', 'Active'
WHERE @math_cs_id IS NOT NULL
  AND NOT EXISTS (
      SELECT 1
      FROM subject_papers
      WHERE curriculum_subject_id = @math_cs_id
        AND paper_code = 'P425/2'
        AND status = 'Active'
  );

-- 7. Current aligned A-Level results must not retain the historical 6-point
--    interpretation, and incomplete paper sets must be Pending.
UPDATE student_subject_results ssr
INNER JOIN enrollments en
    ON en.enrollment_id = ssr.enrollment_id
INNER JOIN examination_subjects es
    ON es.examination_subject_id = ssr.examination_subject_id
INNER JOIN examinations ex
    ON ex.examination_id = es.examination_id
INNER JOIN classes cl
    ON cl.class_id = ex.class_id
INNER JOIN curricula cur
    ON cur.curriculum_code = 'UACE_ALIGNED'
    AND cur.academic_level_id = cl.academic_level_id
    AND cur.status = 'Active'
SET
    ssr.status = 'Pending',
    ssr.grade = NULL,
    ssr.grade_point = NULL,
    ssr.grade_weight = NULL,
    ssr.remarks = CONCAT(
        'Pending: assigned papers and student marks must be complete before the aligned subject grade is calculated.'
    )
WHERE
    (
        SELECT COUNT(*)
        FROM subject_papers sp
        INNER JOIN curriculum_subjects cs
            ON cs.curriculum_subject_id = sp.curriculum_subject_id
        WHERE cs.curriculum_id = cur.curriculum_id
          AND cs.subject_id = es.subject_id
          AND cs.status = 'Active'
          AND sp.status = 'Active'
    )
    <>
    (
        SELECT COUNT(*)
        FROM examination_papers ep
        INNER JOIN subject_papers sp2
            ON sp2.paper_id = ep.paper_id
        INNER JOIN curriculum_subjects cs2
            ON cs2.curriculum_subject_id = sp2.curriculum_subject_id
        WHERE ep.examination_subject_id = ssr.examination_subject_id
          AND ep.status = 'Active'
          AND sp2.status = 'Active'
          AND cs2.curriculum_id = cur.curriculum_id
          AND cs2.subject_id = es.subject_id
    )
    OR
    (
        SELECT COUNT(*)
        FROM marks m
        INNER JOIN examination_papers ep3
            ON ep3.examination_paper_id = m.examination_paper_id
        WHERE ep3.examination_subject_id = ssr.examination_subject_id
          AND ep3.status = 'Active'
          AND m.student_id = en.student_id
    )
    <
    (
        SELECT COUNT(*)
        FROM examination_papers ep4
        WHERE ep4.examination_subject_id = ssr.examination_subject_id
          AND ep4.status = 'Active'
    );

COMMIT;

-- Verification.
SELECT
    sp.paper_id,
    sp.paper_code,
    sp.paper_number,
    sp.paper_name,
    sp.status
FROM subject_papers sp
INNER JOIN curriculum_subjects cs
    ON cs.curriculum_subject_id = sp.curriculum_subject_id
WHERE cs.curriculum_id = @uace_curriculum_id
  AND cs.subject_id = 1
ORDER BY sp.paper_id;

SELECT
    examination_paper_id,
    examination_subject_id,
    paper_id,
    max_score,
    pass_mark,
    status
FROM examination_papers
WHERE paper_id IN (
    SELECT paper_id
    FROM subject_papers
    WHERE curriculum_subject_id = @math_cs_id
      AND paper_code IN ('P425/1','P425/2')
);

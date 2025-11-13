(defun c:closestpairs (/ cnt dst dsts fidx idx idxs len p pts radius res ss ti total)

  ;; Initialize
  (setq total 0)
  (setclear)
  (setq ti (car (_VL-TIMES)))
  (setq radius 70.0)

  ;; Function: GetPoints – returns a list of point coordinates from a selection set
  (defun GetPoints (ss / e i l)
    (setq i -1)
    (if ss
      (while (setq e (ssname ss (setq i (1+ i))))
        (setq l (cons (cdr (assoc 10 (entget e))) l))
      )
    )
    l
  )

  ;; Gather all points
  (setq pts (GetPoints (ssget "_X" '((0 . "POINT")))))

  ;; Build KD-tree for efficient spatial searches
  (kdtreecreate pts)

  (setq fidx -1)

  ;; Main loop – iterate through points
  (foreach p pts
    (setq res (kdtreeradiusSearch p (* radius radius)))
    (setq idxs (nth 0 res))
    (setq dsts (nth 1 res))
    (setq len (length idxs))
    (setq cnt 0)

    (while (< cnt len)
      (setq idx (nth cnt idxs))
      (setq dst (nth cnt dsts))

      (if (= dst 0)
        (setq fidx idx)
        (progn
          (setinsert (list idx fidx))
          (if (not (setcontains (list fidx idx)))
            (progn
              (grdraw p (nth idx pts) 3 0)
              (setq total (1+ total))
            )
          )
        )
      )
      (setq cnt (1+ cnt))
    )
  )

  ;; Print timing and summary
  (princ
    (strcat
      "\nTotal "
      (itoa total)
      " "
      (rtos (/ (- (car (_VL-TIMES)) ti) 1000.0) 2 4)
      " sec."
    )
  )
  
  ;; free
  (setclear)
  (kdtreedestroy)
  (print)
)


;no grdraw Total 12718 0.8570sec.
;with grdraw Total 12718 1.9190sec.
let shiftl x a =
  loop x = x and
       i = 0 in
    if i < a then
      recur (x*2) (i+1)
    else
      x
    end
  end
end

let maskextract x s a =
  let x = shiftl (x) (64 +- (s + a)) in
    loop x = x and
         r = 0 and
         a = a in
      if a == 0 then
        r
      else
        recur (x*2) (r*2 + (x < 0)) (a+-1)
      end
    end
  end
end

let getqueen board x =
  maskextract (board) (x*4) (4)
end

let addqueen board x y =
  board + (shiftl (y) (x*4))
end

let threatens lx ly rx ry =
  let dx = rx +- lx in
    (ry == ly) || (ry == ly + dx) || (ry == ly +- dx)
  end
end

let canaddqueen board x y =
  loop i = 0 in
    if i == x then
      1
    else
      if threatens (i) (getqueen (board) (i)) (x) (y) then
        0
      else
        recur (i+1)
      end
    end
  end
end

let eightqueens board n x =
  if x == n then
    board
  else
    loop y = 0 in
      if y == n then
        -1
      else
        if canaddqueen (board) (x) (y) then
          let board = addqueen (board) (x) (y) and
              board = eightqueens (board) (n) (x+1) in
            if board < 0 then
              recur (y+1)
            else
              board
            end
          end
        else
          recur (y+1)
        end
      end
    end
  end
end

let main n =
  eightqueens (0) (n) (0)
end

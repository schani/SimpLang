let leadingzeros x =
  if x == 0 then
    64
  else
    if x < 0 then
      0
    else
      1 + (leadingzeros (x*2))
    end
  end
end

let main x =
  leadingzeros (x)
end

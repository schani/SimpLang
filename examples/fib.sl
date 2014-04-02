let fib n =
  if n < 2 then
    n
  else
    (fib (n+-1)) + (fib (n+-2))
  end
end

let main n =
  fib (n)
end

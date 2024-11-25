function media(t)
    local sum = 0
    for k,v in ipairs(t) do
        sum = sum + v
    end

    return sum/table.getn(t)
end

local linhas = {}
local ils = {}
local tempos = {}
for linha in io.lines("resultados.txt") do table.insert (linhas, linha) end
for i, linha in ipairs(linhas) do 
    if (i % 2 == 0) then
        table.insert(tempos, linha) 
    else
        table.insert(ils, linha) 
    end
end

print(media(ils), media(tempos))




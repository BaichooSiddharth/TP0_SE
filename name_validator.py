with open("./names.txt") as f:
    for i,n in enumerate(f.readlines()):
        temp = n[:-1]
        temp = temp.split(" ")
        print(f"TEAMMEMBER:{{{' '.join(temp[:-1])}}}\nMATRICULE:{{{temp[-1]}}}")

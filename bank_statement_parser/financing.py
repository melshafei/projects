
import os

account = "Visa"
#account = "Checking"
dir = "C:\\Users\\Mahmoud\\Documents\\Files & Documents\\money tracking\\"+account

def parseCatagory(desc):
    import rules
    for cat in rules.dicriptions:
        for tag in rules.dicriptions[cat]:
            if tag.lower() in desc.lower():
                return cat
    return "Other"

def parseSpending(data):
    spending = 0.0
    if data[2] != '':
        spending += float(data[2])
    if data[3] != '':
        spending -= float(data[3])
    return spending

def processFile(fname, outFile, outList):
    lines = [line.rstrip('\n') for line in open(fname)]
    for line in lines:
        data = line.split(',')
        if (data[0]) == "":
            continue
        while len(data) > 5:
            data[1] += data[2]
            data.remove(data[2])
        print(data)
        date = data[0].split('/')
        datestr = "%s/%s/%s" % (date[1], date[0], date[2])
        spending = parseSpending(data)
        cat = parseCatagory(data[1])
        newline = "%s,%s,%s,%f,%s" % (datestr,cat,data[1],spending,account)
        print(newline)
        if (outList.count(newline) == 0):
            outList.append(newline);
            outFile.write(newline+"\n")
    return

#fname = "C:\\Users\\Mahmoud\\Documents\\Files & Documents\\money tracking\\bs.v.2014.7-9.csv"
outList = []
outfile = open(dir+"\\out.csv", 'w')
for filename in os.listdir(dir):
    if filename.endswith(".csv") and filename != "out.csv":
        processFile(dir+"\\"+filename, outfile, outList)
outfile.close()
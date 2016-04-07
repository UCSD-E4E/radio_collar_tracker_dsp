# -* python *-
import argparse

parser = argparse.ArgumentParser(description="Run Number Generator")
parser.add_argument("destination")
parser.add_argument("-e", "--echo", action='store_true')
args = parser.parse_args()

fileCountFile = open("%s/fileCount" % args.destination, "r")
runNum = int(fileCountFile.readline().strip().split()[1])
fileCountFile.close()
print runNum

if not args.echo:
    fileCountFile = open("%s/fileCount" % args.destination, "w")
    fileCountFile.write("currentRun: %d\n" % (runNum + 1))
    fileCountFile.close()


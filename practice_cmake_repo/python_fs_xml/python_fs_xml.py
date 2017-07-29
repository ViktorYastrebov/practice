"""My first application on python under Visual Studio"""

import os
import zipfile
import xml.etree.cElementTree as ElemTree

class FileList:
  'FileList implementation'
  #members
  entry_ = ""

  #methods
  def __init__(self, entry):
      FileList.entry_ = entry

  def foldEntries(self):
      list = []
      for subdir, dirs, files in os.walk(FileList.entry_):
        for file in files:
           realationPath = "." + subdir[len(FileList.entry_):]
           list.append((realationPath, file))
      return list

  def toZip(self, tupleList, zipFile) :
      with zipfile.ZipFile(zipFile, 'w') as zipper:
        for item in tupleList:
            merged = os.path.join(item[0], item[1])
            zipper.write(os.path.join(FileList.entry_, merged),merged)

  def toXml(self, tupleList, xmlFile):
     root = ElemTree.Element("root")
     root.text = FileList.entry_
     for item in tupleList:
        merged = os.path.join(item[0], item[1])
        xmlItem = ElemTree.SubElement(root, "entry")
        xmlItem.text = merged
     xmlTree = ElemTree.ElementTree(root)
     xmlTree.write(xmlFile)

fileList = FileList("c:\\Users\\Veleos\\Documents\\temp\\test_folder")
fileList.toZip(fileList.foldEntries(), "c:\\Users\\Veleos\\Documents\\temp\\test_folder_py.zip")
fileList.toXml(fileList.foldEntries(), "c:\\Users\\Veleos\\Documents\\temp\\test_folder.xml")



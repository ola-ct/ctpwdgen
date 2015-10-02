/*

    Copyright (c) 2015 Oliver Lau <ola@ct.de>, Heise Medien GmbH & Co. KG

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "presets.h"

const Preset::TemplateCharacterMap Preset::TemplateCharacters = {
  std::pair<char, QString>('V', "AEIOUY"),
  std::pair<char, QString>('v', "aeiuoy"),
  std::pair<char, QString>('C', "BCDFGHJKLMNPQRSTVWXZ"),
  std::pair<char, QString>('c', "bcdfghjklmnpqrstvwxz"),
  std::pair<char, QString>('A', "ABCDEFGHIJKLMNOPQRSTUVWXYZ"),
  std::pair<char, QString>('a', "abcdefghiJklmnopqrstuvwxyz"),
  std::pair<char, QString>('n', "0123456789"),
  std::pair<char, QString>('o', "@&%?,=[]_:-+*$#!'^~;()/."),
  std::pair<char, QString>('x', "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789@&%?,=[]_:-+*$#!'^~;()/.")
};


const Preset::PresetType Preset::Presets = {
  std::pair<QString, Preset>(QObject::tr("Extreme security (32 chars)"), Preset({
    "Aanoxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
  }, true)),
  std::pair<QString, Preset>(QObject::tr("Very high security (24 chars)"), Preset({
    "Aanoxxxxxxxxxxxxxxxxxxxx"
  }, true)),
  std::pair<QString, Preset>(QObject::tr("High security (18 chars)"), Preset({
    "Aanoxxxxxxxxxxxxxx"
  }, true)),
  std::pair<QString, Preset>(QObject::tr("Medium security (12 chars)"), Preset({
    "Aanoxxxxxxxx"
  }, true)),
  std::pair<QString, Preset>(QObject::tr("Low security (6 chars)"), Preset({
    "Aanoxx"
  }, true)),
  std::pair<QString, Preset>(QObject::tr("High security (18 chars, easy to type)"), Preset({
    "CvcvnoCvcvCvcvCvcv",
    "CvcvCvcvnoCvcvcvno",
    "CvcvCvcvCvcvnocvCv",
    "CvccnoCvcvCvcvCvcv",
    "CvccCvcvnoCvcvcvno",
    "CvccCvcvCvcvnocvCv",
    "CvcvnoCvccCvcvCvcc",
    "CvcvCvccnoCvcvccno",
    "CvcvCvccCvcvnoccCv",
    "CvcvnoCvcvCvccCvcv",
    "CvcvCvcvnoCvcccvno",
    "CvcvCvcvCvccnocvCv",
    "CvccnoCvccCvcvCvcc",
    "CvccCvccnoCvcvccno",
    "CvccCvccCvcvnoccCv",
    "CvcvnoCvccCvccCvcc",
    "CvcvCvccnoCvccccno",
    "CvcvCvccCvccnoccCv",
    "CvccnoCvcvCvccCvcv",
    "CvccCvcvnoCvcccvno",
    "CvccCvcvCvccnocvCv",
    "CVVVCvvvnnnnCvcvvo",
    "ocvvcvvvCvCvCvvvCv",
    "cvcvnoCvcvcvcvCvcv",
    "cvcvCvcvnocvcvcvno",
    "cvcvCvcvCvcvnocvCv",
    "cvccnoCvcvcvcvCvcv",
    "cvccCvcvnocvcvcvno",
    "cvccCvcvCvcvnocvCv",
    "cvcvnoCvcccvcvCvcc",
    "cvcvCvccnocvcvccno",
    "cvcvCvccCvcvnoccCv",
    "cvcvnoCvcvcvccCvcv",
    "cvcvCvcvnocvcccvno",
    "cvcvCvcvCvccnocvCv",
    "cvccnoCvcccvcvCvcc",
    "cvccCvccnocvcvccno",
    "cvccCvccCvcvnoccCv",
    "cvcvnoCvcccvccCvcc",
    "cvcvCvccnocvccccno",
    "cvcvCvccCvccnoccCv",
    "cvccnoCvcvcvccCvcv",
    "cvccCvcvnocvcccvno",
    "cvccCvcvCvccnocvCv",
    "cVVVCvvvnnnnCvcvvo",
  }, false)),
  std::pair<QString, Preset>(QObject::tr("Medium security (12 chars, easy to type)"), Preset({
    "CvcvnoCvcvcv",
    "CvcvCvCvcvno",
    "CvcvCvcvnoCv",
    "CvccnoCvcvcv",
    "CvccCvCvcvno",
    "CvccCvcvnoCv",
    "CvcvnoCvcvcc",
    "CvcvCvCvcvno",
    "CvcvCvcvnoCv",
    "CvcvnoCvcccv",
    "CvcvCvCvccno",
    "CvcvCvccnoCv",
    "CvccnoCvcvcc",
    "CvccCvCvcvno",
    "CvccCvcvnoCv",
    "CvcvnoCvcccc",
    "CvcvCvCvccno",
    "CvcvCvccnoCv",
    "CvccnoCvcccv",
    "CvccCvCvccno",
    "CvccCvccnoCv",
    "CVVVCvnnCvvo",
    "ocvvcvCvCvCv",
  }, false)),
  std::pair<QString, Preset>(QObject::tr("Basic security (8 chars, easy to type)"), Preset({
    "noCvcvcv",
    "CvCvcvno",
    "CvcvnoCv",
    "noCvcvcv",
    "CvCvcvno",
    "CvcvnoCv",
    "noCvcvcc",
    "CvCvcvno",
    "CvcvnoCv",
    "noCvcccv",
    "CvCvccno",
    "CvccnoCv",
    "noCvcvcc",
    "CvCvcvno",
    "CvcvnoCv",
    "noCvcccc",
    "CvCvccno",
    "CvccnoCv",
    "noCvcccv",
    "CvCvccno",
    "CvccnoCv",
    "CvnnCvvo",
    "cvCvvvCn",
  }, false)),
  std::pair<QString, Preset>(QObject::tr("4-digit PIN"), Preset({
    "nnnn"
  }, false)),
  std::pair<QString, Preset>(QObject::tr("5-digit PIN"), Preset({
    "nnnnn"
  }, false))
};


const QString &Preset::charSetFor(char ch)
{
  return TemplateCharacters[ch];
}


const Preset &Preset::presetFor(const QString &id)
{
  return Presets[id];
}

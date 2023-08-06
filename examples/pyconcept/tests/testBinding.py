'''Test Concept Text API'''
import os
import json
import unittest
from zipfile import ZipFile

import pyconcept as pc


def _read_trs(file_path: str) -> str:
    '''Read JSON from TRS file'''
    with ZipFile(file_path, 'r') as archive:
        json_data = archive.read('document.json')
    return json_data


class TestBinding(unittest.TestCase):
    '''Test class for Concept API.'''

    def test_convert_to_ascii(self):
        ''' Test converting to ASCII syntax '''
        self.assertEqual(pc.convert_to_ascii(''), '')
        self.assertEqual(pc.convert_to_ascii(r'1=1'), r'1 \eq 1')
        self.assertEqual(pc.convert_to_ascii(r'1 \eq 1'), r'1 \eq 1')
        self.assertEqual(pc.convert_to_ascii(r'B(X1)'), r'B(X1)')
        self.assertEqual(pc.convert_to_ascii('\u212c(X1)'), r'B(X1)')

    def test_convert_to_math(self):
        ''' Test converting to MATH syntax '''
        self.assertEqual(pc.convert_to_math(''), '')
        self.assertEqual(pc.convert_to_math(r'1=1'), r'1=1')
        self.assertEqual(pc.convert_to_math(r'1 \eq 1'), r'1=1')
        self.assertEqual(pc.convert_to_math(r'B(X1)'), '\u212c(X1)')

    def test_parse_expression(self):
        ''' Test parsing expression '''
        out1 = json.loads(pc.parse_expression(''))
        self.assertEqual(out1['parseResult'], False)

        out2 = json.loads(pc.parse_expression('X1=X2'))
        self.assertEqual(out2['parseResult'], True)
        self.assertEqual(out2['syntax'], 'math')

    def test_check_schema(self):
        ''' Test checking schema '''
        schema = self._get_default_schema()
        output = pc.check_schema(schema)
        self.assertTrue(output != '')

    def test_check_expression(self):
        ''' Test checking expression against given schema '''
        schema = self._get_default_schema()
        out1 = json.loads(pc.check_expression(schema, 'X1=X1'))
        self.assertEqual(out1['parseResult'], True)

        out2 = json.loads(pc.check_expression(schema, 'X1=X2'))
        self.assertEqual(out2['parseResult'], False)

    def test_reset_aliases(self):
        ''' Test reset aliases in schema '''
        schema = self._get_default_schema()
        fixedSchema = json.loads(pc.reset_aliases(schema))
        self.assertTrue(len(fixedSchema['items']) > 1)
        self.assertEqual(fixedSchema['items'][1]['alias'], 'S1')

    def _get_default_schema(self) -> str:
        file_path = os.getcwd() + r'/tests/data/КС начальник-подчиненный.trs'
        return _read_trs(file_path)


if __name__ == '__main__':
    unittest.main()

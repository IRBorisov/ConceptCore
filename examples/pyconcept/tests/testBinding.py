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

    def _get_default_schema(self) -> str:
        file_path = os.getcwd() + r'/tests/data/КС начальник-подчиненный.trs'
        return _read_trs(file_path)

    def test_check_schema(self):
        ''' Test checking schema '''
        schema = self._get_default_schema()
        output = pc.check_schema(schema)
        self.assertTrue(output != '')

    def test_parse_expression(self):
        ''' Test checking schema '''
        schema = self._get_default_schema()
        out1 = json.loads(pc.parse_expression(schema, 'X1=X1'))
        self.assertEqual(out1['parseResult'], True)

        out2 = json.loads(pc.parse_expression(schema, 'X1=X2'))
        self.assertEqual(out2['parseResult'], False)

if __name__ == '__main__':
    unittest.main()

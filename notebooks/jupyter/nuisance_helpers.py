from xml.etree import ElementTree as ET
class nuiscard:
    def __init__(self):
        self.root = ET.Element("nuisance")

    def sample(self, name, inputs, **kwargs):
        
        el = ET.SubElement(self.root, "sample", name=name, input=inputs)
        for k in kwargs:
            el.set(k, str(kwargs[k]))

    def parameter(self, types, name, nominal, **kwargs):
        el = ET.SubElement(self.root, "parameter", name=name, type=types, nominal=nominal)
        for k in kwargs:
            el.set(k, str(kwargs[k]))

    def config(self, **kwargs):
        for k in kwargs:
            el = ET.SubElement(self.root, "config")
            el.set(k, str(kwargs[k]))
        
    def write(self, name="nuismin.xml"):
        tree = ET.ElementTree(self.root)
        ET.indent(tree, '    ')
        
        tree.write(name, encoding='utf-8', xml_declaration=True)


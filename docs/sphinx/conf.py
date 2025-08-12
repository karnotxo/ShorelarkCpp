import os
import sys

project = 'CShorelark'
copyright = '2024, CShorelark Team'
author = 'CShorelark Team'
release = '0.1.0'

# Add path to extensions
sys.path.append(os.path.abspath('.'))

# Extensions
extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.intersphinx',
    'sphinx.ext.viewcode',
    'sphinx.ext.graphviz',
    'breathe',
    'sphinxcontrib.mermaid',
    'sphinx_multiversion',
]

# Breathe configuration
breathe_projects = {
    "CShorelark": "../build/docs/api/xml"
}
breathe_default_project = "CShorelark"

# HTML theme settings
html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']
html_logo = '../assets/logo.png'
html_theme_options = {
    'logo_only': False,
    'display_version': True,
    'prev_next_buttons_location': 'bottom',
    'style_external_links': False,
    'style_nav_header_background': '#2980B9',
    'collapse_navigation': True,
    'sticky_navigation': True,
    'navigation_depth': 4,
    'includehidden': True,
    'titles_only': False
}

# Mermaid configuration
mermaid_params = [
    '--theme', 'default',
    '--width', '100%',
    '--backgroundColor', 'transparent'
]

# Other settings
source_suffix = '.rst'
master_doc = 'index'
language = 'en'
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']
pygments_style = 'sphinx' 
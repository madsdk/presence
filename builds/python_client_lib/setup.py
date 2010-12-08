from distutils.core import setup

setup(name='Presence Client Library',
      version='VERSION',
      description='A consumer-only Presence library.',
      author='Mads D. Kristensen',
      author_email='madsk@cs.au.dk',
      url='https://github.com/madsdk/presence',
      packages=['presence', 'presence.packed_data']
)

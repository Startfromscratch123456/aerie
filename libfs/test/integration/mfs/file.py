#
# FILE TESTS
#


import testfw
import os 


def addIntegrationTests(env, parent_dir, testProgram, serverProgram):
    #
    # FILE TESTS (SINGLE CLIENTS)
    #

    env.addIntegrationTest(testfw.integration_test.IntegrationTest(
        name = 'MFSFile:TestCreateUnlink',
        init_script = os.path.join(parent_dir, 'test/integration/mfs/init.sh'),
        testfw = testProgram, server = serverProgram,
        clients = { 
            'C1': ( testProgram, [('T1', 'MFSFile:TestCreateUnlink')]),
        },
        rendezvous = []
    ))
    
    
    #
    # FILE TESTS (CONCURRENT CLIENTS)
    #

    env.addIntegrationTest(testfw.integration_test.IntegrationTest(
        name = 'MFSFile:TestCreateOpenConcurrent',
        init_script = os.path.join(parent_dir, 'test/integration/mfs/init.sh'),
        testfw = testProgram, server = serverProgram,
        clients = { 
            'C1': ( testProgram, [('T1', 'MFSFile:TestCreate')]),
            'C2': ( testProgram, [('T1', 'MFSFile:TestOpen')])
        },
        rendezvous = [('C1:T1:E2:block', 'C2:T1:E1:block'), 
                      ('C1:T1:E3:block', 'C2:T1:E3:block')]
    ))

    env.addIntegrationTest(testfw.integration_test.IntegrationTest(
        name = 'MFSFile:TestCreateOpenConcurrent2',
        init_script = os.path.join(parent_dir, 'test/integration/mfs/init.sh'),
        testfw = testProgram, server = serverProgram,
        clients = { 
            'C1': ( testProgram, [('T1', 'MFSFile:TestCreate2')]),
            'C2': ( testProgram, [('T1', 'MFSFile:TestRead2')])
        },
        rendezvous = [('C1:T1:E2:block', 'C2:T1:E1:block'), 
                      ('C1:T1:E3:block', 'C2:T1:E3:block')]
    ))

    env.addIntegrationTest(testfw.integration_test.IntegrationTest(
        name = 'MFSFile:TestCreateOpenConcurrent3',
        init_script = os.path.join(parent_dir, 'test/integration/mfs/init.sh'),
        testfw = testProgram, server = serverProgram,
        clients = { 
            'C1': ( testProgram, [('T1', 'MFSFile:TestCreate3')]),
            'C2': ( testProgram, [('T1', 'MFSFile:TestRead3')])
        },
        rendezvous = [('C1:T1:E2:block', 'C2:T1:E1:block'), 
                      ('C1:T1:E3:block', 'C2:T1:E3:block')]
    ))


    env.addIntegrationTest(testfw.integration_test.IntegrationTest(
        name = 'MFSFile:TestCreateUnlinkConcurrent',
        init_script = os.path.join(parent_dir, 'test/integration/mfs/init.sh'),
        testfw = testProgram, server = serverProgram,
        clients = { 
            'C1': ( testProgram, [('T1', 'MFSFile:TestCreate')]),
            'C2': ( testProgram, [('T1', 'MFSFile:TestUnlink')])
        },
        rendezvous = [('C1:T1:E2:block', 'C2:T1:E1:block'), 
                      ('C1:T1:E3:block', 'C2:T1:E3:block')]
    ))

    env.addIntegrationTest(testfw.integration_test.IntegrationTest(
        name = 'MFSFile:TestCreateUnlinkConcurrent2',
        init_script = os.path.join(parent_dir, 'test/integration/mfs/init.sh'),
        testfw = testProgram, server = serverProgram,
        clients = { 
            'C1': ( testProgram, [('T1', 'MFSFile:TestCreate')]),
            'C2': ( testProgram, [('T1', 'MFSFile:TestUnlinkCreate')])
        },
        rendezvous = [('C1:T1:E2:block', 'C2:T1:E1:block'), 
                      ('C1:T1:E3:block', 'C2:T1:E3:block')]
    ))



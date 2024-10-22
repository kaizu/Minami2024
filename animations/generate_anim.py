import bpy
import math
import colorsys
import csv

NUM = 8


def load_data():
    matids = [[]]
    bpy.ops.mesh.primitive_plane_add(size=16.0)
    matids[-1].append(0)
    mats = make_materials(NUM*2)

    frame_interval = 1
    bpy.context.scene.frame_start = 0
    bpy.context.scene.frame_end = 501
    # bpy.context.scene.frame_end = 0  #XXX: Screenshot
    bpy.context.scene.frame_current = 0
    
    matiddict = dict(A=2, B=6, C=10, X=14)
    
    filename = "C:\\Users\\kaizu\\Desktop\\result_15_0_0.3_9.6_692_1e-9_3.csv"  # Path to the input data
    tracer_diameter = 15
    
    with open(filename) as f:
        prev = None
        idx = -1
        frame = 0
        for row in csv.reader(f):
            idx += 1
            if row[0].startswith("#"):
                continue
            t, sp, spid, x, y, z = row
            if prev != t:
                # if prev is not None:
                #     break
                if frame > bpy.context.scene.frame_end * frame_interval:
                    break
                if prev is not None:
                    frame += 1
                    # break  #XXX: Screenshot
                prev = t
                if frame % frame_interval == 0:
                    bpy.context.scene.frame_set(frame // frame_interval)
                    matids.append([0])
            spid, x, y, z = int(spid), float(x), float(y), float(z)
            # found = int(found)

            # if spid % 10 != 1:
            #     continue
            #if sp == "B" and idx % 5 != 0:
            #    continue
            #if idx > 300:
            #    break
            #if sp == "B":
            #    continue

            scale = 35
            if sp == "X":
                x, y, z = x % (2 * 0.149), y % 0.149, z % 0.149
            loc = ((x - 0.149 * 1.0) * scale, (y - 0.149 * 0.5) * scale, z * scale + 0.2)
            name = f"Sphere.{spid}"
            if frame == 0:
                # diameter = 0.010 if sp == "X" else 0.005
                diameter = tracer_diameter / 1000 if sp == "X" else 0.0096
                bpy.ops.mesh.primitive_uv_sphere_add(location=loc, radius=diameter * scale * 0.5)
                sphere = bpy.context.object
                sphere.name = name
                # sphere.scale = (diameter * scale, diameter * scale, diameter * scale)
                bpy.ops.object.material_slot_add()
                
                matid = 14 if sp == "X" else 2
                matids[-1].append(matid)
                # matids.append(matiddict[sp])
                ## matids.append(matiddict.get(sp, 0))
                
                if mats is None:
                    mats = make_materials(NUM*2)
                bpy.context.object.active_material = mats[matids[-1][-1]].copy()
                #bpy.context.object.active_material = mats[matids[-1][-1]]
                sphere.keyframe_insert(data_path="location")

                if sp != "X":
                    mat = sphere.material_slots[0].material
                    basecolor = mat.node_tree.nodes['Principled BSDF'].inputs[0]
                    basecolor.keyframe_insert(data_path="default_value")
                
            elif frame % frame_interval == 0:
                sphere = bpy.data.objects[name]
                sphere.location = loc
                #sphere.location.x += 5
                sphere.keyframe_insert(data_path="location")
                
                if sp != "X":
                    mat = sphere.material_slots[0].material
                    basecolor = mat.node_tree.nodes['Principled BSDF'].inputs[0]
                    # if found == 1:
                    #     basecolor.default_value = mats[10].node_tree.nodes['Principled BSDF'].inputs[0].default_value
                    basecolor.keyframe_insert(data_path="default_value")
                    
                matid = 14 if sp == "X" else 10
                matids[-1].append(matid)
                
    bpy.context.scene.frame_set(0)
    
def delete_all():
    for i in bpy.data.meshes:
        bpy.data.meshes.remove(i)
    for i in bpy.data.materials:
        bpy.data.materials.remove(i)
    for i in bpy.data.lights:
        bpy.data.lights.remove(i)

def make_cube(x, y, z):
    loc = (x, y, z)
    # bpy.ops.mesh.primitive_plane_add(size=12.0)
    # bpy.ops.mesh.primitive_cube_add(location=loc, size=0.8)
    # bpy.ops.object.material_slot_add()
    bpy.ops.mesh.primitive_uv_sphere_add(location=loc)
    sphere = bpy.context.object
    sphere.scale = (0.4, 0.4, 0.4)
    #sphere.data.materials.append(material)
    bpy.ops.object.material_slot_add()

def make_cubes(num):
    for x in range(-num, num):
        make_cube(x, 0, 1)

def make_transparent_material(name):
    ma = bpy.data.materials.new(name)
    ma.use_nodes = True

    node_tree = ma.node_tree
    output = node_tree.nodes['Material Output']
    p_bsdf = node_tree.nodes['Principled BSDF']
    ma.blend_method = 'BLEND'
    p_bsdf.inputs['Base Color'].default_value = (0.0, 1.0, 0.0, 1.0)
    p_bsdf.inputs['Alpha'].default_value = 0.05

    return ma

def make_material(name, color):
    ma = bpy.data.materials.new(name)
    ma.use_nodes = True
    bsdf = ma.node_tree.nodes['Principled BSDF']
    bsdf.inputs[0].default_value = color
    bsdf.inputs[4].default_value = 0.7#1
    bsdf.inputs[6].default_value = 0.5
    bsdf.inputs[7].default_value = 0.7#1
    bsdf.inputs[9].default_value = 0
    bpy.context.object.data.materials.append(ma)
    ma.diffuse_color = color
    return ma


def make_materials(num):
    mats = []
    mats.append(make_transparent_material("transparent"))
    mats.append(make_material("silver", (190/256, 190/256, 190/256, 1)))
    for i in range(num):
        name = 'color'+str(i)
        rgb = colorsys.hsv_to_rgb(i/num*0.8, 0.8, 0.8)
        # rgb = colorsys.hsv_to_rgb(i/num, 1, 1)
        color = (rgb[0], rgb[1], rgb[2], 1)
        mats.append(make_material(name, color))
    return mats


def set_materials(mats):
    index = 0
    for o in bpy.data.objects:
        for m in o.material_slots:
            m.material = mats[index]
            index += 1
            break


def set_camera():
    camera = bpy.data.objects['Camera']
    x = math.radians(65)
    y = 0
    z = math.radians(22)
    rot = (x, 0, 0)
    loc = (0, -24, 14)
    # rot = (x, 0, z)
    # loc = (9, -24, 14)
    camera.location = loc
    camera.rotation_euler = rot


if __name__ == '__main__':
    delete_all()
    
    load_data()
    
    #make_cubes(NUM)
    #mats = make_materials(NUM*2)
    #set_materials(mats)
    set_camera()

    light_data = bpy.data.lights.new(name="light_2.80", type='POINT')
    light_data.energy = 4000
    # create new object with our light datablock
    light_object = bpy.data.objects.new(name="light_2.80", object_data=light_data)
    # link light object
    bpy.context.collection.objects.link(light_object)
    # make it active 
    bpy.context.view_layer.objects.active = light_object
    #change location
    light_object.location = (0, -8, 8)
    #light_object.location = (5, -8, 8)
    #light_object.location = (5, -4, 10)

    bpy.context.scene.render.image_settings.file_format = 'PNG'
    #bpy.ops.render.render()
    #D = bpy.data
    #D.images['Render Result'].save_render(filepath = 'C:\\Users\\kaizu\\Desktop\\output.png')
    
    bpy.context.scene.render.filepath = 'C:\\Users\\kaizu\\Desktop\\outputs\\output'  # Output path
    bpy.ops.render.render(animation=True)
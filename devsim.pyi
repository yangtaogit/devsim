from typing import Any

from typing import overload

class error(Exception): ...

def add_1d_contact(material, mesh, name, tag) -> Any: ...
def add_1d_interface(mesh, tag, name) -> Any: ...
def add_1d_mesh_line(mesh, tag, pos, ns, ps) -> Any: ...
def add_1d_region(mesh, tag1, tag2, region, material) -> Any: ...
def add_2d_contact(name, material, mesh, region, xl, xh, yl, yh, bloat) -> Any: ...
def add_2d_interface(mesh, name, region0, region1, xl, xh, yl, yh, bloat) -> Any: ...
def add_2d_mesh_line(mesh, pos, ns, ps) -> Any: ...
def add_2d_region(mesh, region, material, xl, xh, yl, yh, bloat) -> Any: ...
def add_circuit_node(name, value, variable_update) -> Any: ...
def add_db_entry(material, parameter, value, unit, description) -> Any: ...
def add_gmsh_contact(gmsh_name, material, mesh, name, region) -> Any: ...
def add_gmsh_interface(gmsh_name, mesh, name, region0, region1) -> Any: ...
def add_gmsh_region(gmsh_name, mesh, region, material) -> Any: ...
def circuit_alter(name, param, value) -> Any: ...
def circuit_element(name, value, n1, n2, acreal, acimag) -> Any: ...
def circuit_node_alias(node, alias) -> Any: ...
def close_db() -> Any: ...
def contact_edge_model(device, contact, name, equation, display_type) -> Any: ...
def contact_equation(device, contact, name, circuit_node, edge_charge_model, edge_current_model, edge_model, edge_volume_model, element_charge_model, element_current_model, element_model, volume_node0_model, volume_node1_model, node_charge_model, node_current_model, node_model) -> Any: ...
def contact_node_model(device, contact, name, equation, display_type) -> Any: ...
def create_1d_mesh(mesh) -> Any: ...
def create_2d_mesh(mesh) -> Any: ...
def create_contact_from_interface(device, region, interface, material, name) -> Any: ...
def create_db(filename) -> Any: ...
def create_device(mesh, device) -> Any: ...
def create_gmsh_mesh(mesh, file, coordinates, elements, physical_names) -> Any: ...
def create_interface_from_nodes(device, name, region0, region1, nodes0, nodes1) -> Any: ...
def custom_equation(name, procedure) -> Any: ...
def cylindrical_edge_couple(device, region) -> Any: ...
def cylindrical_node_volume(device, region) -> Any: ...
def cylindrical_surface_area(device, region) -> Any: ...
def debug_triangle_models(device, region) -> Any: ...
def delete_contact_equation(device, contact, name) -> Any: ...
def delete_device(device) -> Any: ...
def delete_edge_model(device, region, name) -> Any: ...
def delete_element_model(device, region, name) -> Any: ...
def delete_equation(device, region, name) -> Any: ...
def delete_interface_equation(device, interface, name) -> Any: ...
def delete_interface_model(device, interface, name) -> Any: ...
def delete_mesh(mesh) -> Any: ...
def delete_node_model(device, region, name) -> Any: ...
@overload
def edge_average_model(device, region, node_model, edge_model, derivative, average_type) -> Any: ...
@overload
def edge_average_model(device = ..., region = ..., node_model = ..., edge_model = ..., average_type = ...) -> Any: ...
@overload
def edge_average_model(device = ..., region = ..., node_model = ..., edge_model = ..., average_type = ..., derivative = ...) -> Any: ...
@overload
def edge_from_node_model(device, region, node_model) -> Any: ...
@overload
def edge_from_node_model(device = ..., region = ..., node_model = ...) -> Any: ...
def edge_model(device, region, name, equation, display_type) -> Any: ...
def edge_solution(device, region, name) -> Any: ...
def element_from_edge_model(device, region, edge_model, derivative) -> Any: ...
def element_from_node_model(device, region, node_model) -> Any: ...
def element_model(device, region, name, equation, display_type) -> Any: ...
def element_pair_from_edge_model(device, region, edge_model, derivative) -> Any: ...
def element_solution(device, region, name) -> Any: ...
def equation(device, region, name, variable_name, node_model, edge_model, edge_volume_model, time_node_model, element_model, volume_node0_model, volume_node1_model, variable_update) -> Any: ...
def finalize_mesh(mesh) -> Any: ...
def get_circuit_equation_number(node) -> Any: ...
def get_circuit_node_list() -> Any: ...
def get_circuit_node_value(solution, node) -> Any: ...
def get_circuit_solution_list() -> Any: ...
def get_contact_charge(device, contact, equation) -> Any: ...
def get_contact_current(device, contact, equation) -> Any: ...
def get_contact_equation_command(device, contact, name) -> Any: ...
def get_contact_equation_list(device, contact) -> Any: ...
def get_contact_list(device) -> Any: ...
def get_db_entry(material, parameter) -> Any: ...
def get_device_list() -> Any: ...
def get_dimension(device) -> Any: ...
def get_edge_model_list(device, region) -> Any: ...
def get_edge_model_values(device, region, name) -> Any: ...
def get_element_model_list(device, region) -> Any: ...
def get_element_model_values(device, region, name) -> Any: ...
def get_element_node_list(device, region, contact, interface, reorder) -> Any: ...
def get_equation_command(device, region, name) -> Any: ...
def get_equation_list(device, region) -> Any: ...
def get_equation_numbers(device, region, equation, variable) -> Any: ...
def get_interface_equation_command(device, interface, name) -> Any: ...
def get_interface_equation_list(device, interface) -> Any: ...
def get_interface_list(device) -> Any: ...
def get_interface_model_list(device, interface) -> Any: ...
def get_interface_model_values(device, interface, name) -> Any: ...
def get_material(device, region, contact) -> Any: ...
def get_matrix_and_rhs(format) -> Any: ...
def get_node_model_list(device, region) -> Any: ...
def get_node_model_values(device, region, name) -> Any: ...
def get_parameter(device, region, name) -> Any: ...
def get_parameter_list(device, region) -> Any: ...
def get_region_list(device, contact, interface) -> Any: ...
def interface_equation(device, interface, name, name0, name1, interface_model, type) -> Any: ...
def interface_model(device, interface, equation) -> Any: ...
def interface_normal_model(device, region, interface) -> Any: ...
def load_devices(file) -> Any: ...
def node_model(device, region, name, equation, display_type) -> Any: ...
def node_solution(device, region, name) -> Any: ...
def open_db(filename, permissions) -> Any: ...
def print_edge_values(device, region, name) -> Any: ...
def print_element_values(device, region, name) -> Any: ...
def print_node_values(device, region, name) -> Any: ...
def register_function(name, nargs, procedure) -> Any: ...
def reset_devsim() -> Any: ...
def save_db() -> Any: ...
def set_circuit_node_value(solution, node, value) -> Any: ...
def set_edge_values(device, region, name, init_from, values) -> Any: ...
def set_element_values(device, region, name, init_from, values) -> Any: ...
def set_initial_condition(static_rhs, dynamic_rhs) -> Any: ...
def set_material(device, region, contact, material) -> Any: ...
def set_node_value(device, region, name, index, value) -> Any: ...
def set_node_values(device, region, name, init_from, values) -> Any: ...
def set_parameter(device, region, name, value) -> Any: ...
@overload
def solve(type, solver_type, absolute_error, relative_error, maximum_error, charge_error, gamma, tdelta, maximum_iterations, maximum_divergence, frequency, output_node, info, symbolic_iteration_limit) -> Any: ...
@overload
def solve(default20) -> Any: ...
@overload
def solve(default20) -> Any: ...
def symdiff(expr) -> Any: ...
def vector_element_model(device, region, element_model) -> Any: ...
def vector_gradient(device, region, node_model, calc_type) -> Any: ...
def write_devices(file, device, type) -> Any: ...

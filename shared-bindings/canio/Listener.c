/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "shared-bindings/canio/Listener.h"
#include "shared-bindings/canio/Message.h"
#include "common-hal/canio/Listener.h"

#include "py/runtime.h"
#include "py/objproperty.h"

//| class Listener:
//|     """Listens for CAN message
//|
//|     canio.Listener is not constructed directly, but instead by calling the
//|     Listen method of a canio.CAN object."""
//|

//|     def read(self) -> Optional[Message]:
//|         """Reads a message, after waiting up to self.timeout seconds
//|
//|         If no message is received in time, None is returned.  Otherwise,
//|         a Message is returned."""
//|         ...
//|
STATIC mp_obj_t canio_listener_read(mp_obj_t self_in) {
    canio_listener_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_canio_listener_check_for_deinit(self);

    canio_message_obj_t *message = m_new_obj(canio_message_obj_t);
    message->base.type = &canio_message_type;

    if (common_hal_canio_listener_readinto(self, message)) {
        return message;
    } else {
        m_free(message); // message did not escape into vm
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(canio_listener_read_obj, canio_listener_read);

//|     def readinto(self, message: Message) -> bool:
//|         """Returns True (and modifies message) if a message was received
//|         before ``timeout`` seconds elapsed, False otherwise."""
//|         ...
//|
STATIC mp_obj_t canio_listener_readinto(mp_obj_t self_in, mp_obj_t message) {
    canio_listener_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_type_t *type = mp_obj_get_type(message);
    if (type != &canio_message_type) {
        mp_raise_TypeError_varg(translate("expected '%q' but got '%q'"), MP_QSTR_Message, type->name);
    }
    common_hal_canio_listener_check_for_deinit(self);
    return mp_obj_new_bool(common_hal_canio_listener_readinto(self, message));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(canio_listener_readinto_obj, canio_listener_readinto);

//|     def in_waiting(self) -> int:
//|         """Returns the number of messages waiting"""
//|         ...
//|
STATIC mp_obj_t canio_listener_in_waiting(mp_obj_t self_in) {
    canio_listener_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_canio_listener_check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_canio_listener_in_waiting(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(canio_listener_in_waiting_obj, canio_listener_in_waiting);

//|     def __iter__(self):
//|         """Returns self, unless the object is deinitialized"""
//|         ...
//|
//|     def __next__(self):
//|         """Reads a message, after waiting up to self.timeout seconds
//|
//|         If no message is received in time, raises StopIteration.  Otherwise,
//|         a Message is returned."""
//|         ...
//|
STATIC mp_obj_t canio_iternext(mp_obj_t self_in) {
    mp_obj_t result = canio_listener_read(self_in);
    if (result == mp_const_none) {
        return MP_OBJ_STOP_ITERATION;
    }
    return result;
}

//|     def deinit(self) -> None:
//|         """Deinitialize this object, freeing its hardware resources"""
//|         ...
//|
STATIC mp_obj_t canio_listener_deinit(mp_obj_t self_in) {
    canio_listener_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_canio_listener_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(canio_listener_deinit_obj, canio_listener_deinit);

//|     def __enter__(self) -> CAN:
//|         """Returns self, to allow the object to be used in a `with` statement for resource control"""
//|         ...
//|
STATIC mp_obj_t canio_listener_enter(mp_obj_t self_in) {
    canio_listener_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_canio_listener_check_for_deinit(self);
    return self_in;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(canio_listener_enter_obj, canio_listener_enter);

//|     def __exit__(self, unused1, unused2, unused3) -> None:
//|         """Calls deinit()"""
//|         ...
STATIC mp_obj_t canio_listener_exit(size_t num_args, const mp_obj_t args[]) {
    canio_listener_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    common_hal_canio_listener_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(canio_listener_exit_obj, 4, 4, canio_listener_exit);


//|     timeout : float
STATIC mp_obj_t canio_listener_timeout_get(mp_obj_t self_in) {
    canio_listener_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_canio_listener_check_for_deinit(self);
    return mp_obj_new_float(common_hal_canio_listener_get_timeout(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(canio_listener_timeout_get_obj, canio_listener_timeout_get);

STATIC mp_obj_t canio_listener_timeout_set(mp_obj_t self_in, mp_obj_t timeout_in) {
    canio_listener_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_canio_listener_check_for_deinit(self);
    common_hal_canio_listener_set_timeout(self, mp_obj_get_float(timeout_in));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(canio_listener_timeout_set_obj, canio_listener_timeout_set);

STATIC const mp_obj_property_t canio_listener_timeout_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&canio_listener_timeout_get_obj,
              (mp_obj_t)&canio_listener_timeout_set_obj,
              (mp_obj_t)mp_const_none},
};



STATIC const mp_rom_map_elem_t canio_listener_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&canio_listener_enter_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&canio_listener_exit_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&canio_listener_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_in_waiting), MP_ROM_PTR(&canio_listener_in_waiting_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&canio_listener_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&canio_listener_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_timeout), MP_ROM_PTR(&canio_listener_timeout_obj) },
};
STATIC MP_DEFINE_CONST_DICT(canio_listener_locals_dict, canio_listener_locals_dict_table);

const mp_obj_type_t canio_listener_type = {
    { &mp_type_type },
    .name = MP_QSTR_Listener,
    .getiter = mp_identity_getiter,
    .iternext = canio_iternext,
    .locals_dict = (mp_obj_dict_t*)&canio_listener_locals_dict,
};
